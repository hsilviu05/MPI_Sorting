#include "../include/bucket_sort.h"
#include <mpi.h>
#include <vector>
#include <algorithm>

void MPI_BucketSort(std::vector<int>& data, int rank, int size, double& comm_time, double& comp_time)
{
    int total_elements;
    if (rank == 0) {
        total_elements = static_cast<int>(data.size());
    }

    double communication_start = MPI_Wtime();
    MPI_Bcast(&total_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);
    comm_time += MPI_Wtime() - communication_start;

    int global_minimum = 0;
    int global_maximum = 0;

    if (rank == 0 && !data.empty()) {
        global_minimum = *std::min_element(data.begin(), data.end());
        global_maximum = *std::max_element(data.begin(), data.end());
    }

    communication_start = MPI_Wtime();
    MPI_Bcast(&global_minimum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&global_maximum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    comm_time += MPI_Wtime() - communication_start;

    std::vector<int> elements_per_process(size);
    std::vector<int> scatter_offsets(size);

    int base_chunk_size = total_elements / size;
    int leftover_elements = total_elements % size;

    for (int process_index = 0; process_index < size; process_index++) {
        elements_per_process[process_index] = base_chunk_size + (process_index < leftover_elements ? 1 : 0);
    }

    scatter_offsets[0] = 0;
    for (int process_index = 1; process_index < size; process_index++) {
        scatter_offsets[process_index] = scatter_offsets[process_index - 1] + elements_per_process[process_index - 1];
    }

    std::vector<int> local_chunk(elements_per_process[rank]);

    communication_start = MPI_Wtime();
    MPI_Scatterv(
        rank == 0 ? data.data() : nullptr,
        elements_per_process.data(),
        scatter_offsets.data(),
        MPI_INT,
        local_chunk.data(),
        elements_per_process[rank],
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start;

    long long value_range = static_cast<long long>(global_maximum) - static_cast<long long>(global_minimum) + 1;
    long long bucket_width = (value_range + size - 1) / size;

    if (bucket_width <= 0) {
        bucket_width = 1;
    }

    std::vector<std::vector<int>> outgoing_buckets(size);

    double computation_start = MPI_Wtime();
    for (int value : local_chunk) {
        int bucket_owner;

        if (value == global_maximum) {
            bucket_owner = size - 1;
        } else {
            long long normalized_value = static_cast<long long>(value) - static_cast<long long>(global_minimum);
            bucket_owner = static_cast<int>(normalized_value / bucket_width);
        }

        if (bucket_owner < 0) {
            bucket_owner = 0;
        }
        if (bucket_owner >= size) {
            bucket_owner = size - 1;
        }

        outgoing_buckets[bucket_owner].push_back(value);
    }
    comp_time += MPI_Wtime() - computation_start;

    std::vector<int> send_counts(size);
    for (int process_index = 0; process_index < size; process_index++) {
        send_counts[process_index] = static_cast<int>(outgoing_buckets[process_index].size());
    }

    std::vector<int> send_offsets(size);
    send_offsets[0] = 0;
    for (int process_index = 1; process_index < size; process_index++) {
        send_offsets[process_index] = send_offsets[process_index - 1] + send_counts[process_index - 1];
    }

    std::vector<int> send_buffer;
    send_buffer.reserve(local_chunk.size());

    for (int process_index = 0; process_index < size; process_index++) {
        send_buffer.insert(
            send_buffer.end(),
            outgoing_buckets[process_index].begin(),
            outgoing_buckets[process_index].end()
        );
    }

    std::vector<int> receive_counts(size);

    communication_start = MPI_Wtime();
    MPI_Alltoall(
        send_counts.data(),
        1,
        MPI_INT,
        receive_counts.data(),
        1,
        MPI_INT,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start;

    std::vector<int> receive_offsets(size);
    receive_offsets[0] = 0;
    for (int process_index = 1; process_index < size; process_index++) {
        receive_offsets[process_index] = receive_offsets[process_index - 1] + receive_counts[process_index - 1];
    }

    int received_elements = 0;
    for (int count : receive_counts) {
        received_elements += count;
    }

    std::vector<int> local_bucket(received_elements);

    communication_start = MPI_Wtime();
    MPI_Alltoallv(
        send_buffer.data(),
        send_counts.data(),
        send_offsets.data(),
        MPI_INT,
        local_bucket.data(),
        receive_counts.data(),
        receive_offsets.data(),
        MPI_INT,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start;

    computation_start = MPI_Wtime();
    std::sort(local_bucket.begin(), local_bucket.end());
    comp_time += MPI_Wtime() - computation_start;

    int local_bucket_size = static_cast<int>(local_bucket.size());
    std::vector<int> gathered_bucket_sizes(size);

    communication_start = MPI_Wtime();
    MPI_Gather(
        &local_bucket_size,
        1,
        MPI_INT,
        gathered_bucket_sizes.data(),
        1,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start;

    std::vector<int> gather_offsets(size);
    int total_sorted_elements = 0;

    if (rank == 0) {
        gather_offsets[0] = 0;
        for (int process_index = 1; process_index < size; process_index++) {
            gather_offsets[process_index] = gather_offsets[process_index - 1] + gathered_bucket_sizes[process_index - 1];
        }

        for (int bucket_size : gathered_bucket_sizes) {
            total_sorted_elements += bucket_size;
        }

        data.resize(total_sorted_elements);
    }

    communication_start = MPI_Wtime();
    MPI_Gatherv(
        local_bucket.data(),
        local_bucket_size,
        MPI_INT,
        rank == 0 ? data.data() : nullptr,
        rank == 0 ? gathered_bucket_sizes.data() : nullptr,
        rank == 0 ? gather_offsets.data() : nullptr,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start;
}
