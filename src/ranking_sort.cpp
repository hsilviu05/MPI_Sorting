#include "../include/ranking_sort.h"
#include <mpi.h>
#include <vector>
#include <algorithm>

void MPI_RankingSort(std::vector<int>& data, int rank, int size, double& comm_time, double& comp_time) 
{
    int total_elements;

    if (rank == 0)
    {
        total_elements = static_cast<int>(data.size());
    }

    double communication_start_time = MPI_Wtime();
    MPI_Bcast(&total_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);
    comm_time += MPI_Wtime() - communication_start_time;

    std::vector<int> complete_data(total_elements);

    if (rank == 0)
    {
        complete_data = data;
    }

    communication_start_time = MPI_Wtime();
    MPI_Bcast(complete_data.data(), total_elements, MPI_INT, 0, MPI_COMM_WORLD);
    comm_time += MPI_Wtime() - communication_start_time;

    std::vector<int> elements_per_process(size);
    std::vector<int> scatter_offsets(size);

    const int base_chunk_size = total_elements / size;
    const int leftover_elements = total_elements % size;

    for (int process_index = 0; process_index < size; process_index++)
    {
        elements_per_process[process_index] = base_chunk_size + (process_index < leftover_elements ? 1 : 0);
    }

    scatter_offsets[0] = 0;
    for (int process_index = 1; process_index < size; process_index++)
    {
        scatter_offsets[process_index] = scatter_offsets[process_index - 1] + elements_per_process[process_index - 1];
    }

    const int local_elements_count = elements_per_process[rank];
    std::vector<int> local_values(local_elements_count);

    communication_start_time = MPI_Wtime();
    MPI_Scatterv(
        complete_data.data(),
        elements_per_process.data(),
        scatter_offsets.data(),
        MPI_INT,
        local_values.data(),
        local_elements_count,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start_time;

    std::vector<int> local_ranks(local_elements_count);

    double computation_start_time = MPI_Wtime();

    for (int local_index = 0; local_index < local_elements_count; local_index++)
    {
        const int current_value = local_values[local_index];
        const int global_index = scatter_offsets[rank] + local_index;

        int final_position = 0;

        for (int compared_index = 0; compared_index < total_elements; compared_index++)
        {
            if (complete_data[compared_index] < current_value)
            {
                final_position++;
            }
            else if (complete_data[compared_index] == current_value && compared_index < global_index)
            {
                final_position++;
            }
        }

        local_ranks[local_index] = final_position;
    }

    comp_time += MPI_Wtime() - computation_start_time;

    std::vector<int> all_ranks;
    std::vector<int> all_values;

    if (rank == 0)
    {
        all_ranks.resize(total_elements);
        all_values.resize(total_elements);
    }

    communication_start_time = MPI_Wtime();
    MPI_Gatherv(
        local_ranks.data(),
        local_elements_count,
        MPI_INT,
        rank == 0 ? all_ranks.data() : nullptr,
        elements_per_process.data(),
        scatter_offsets.data(),
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    MPI_Gatherv(
        local_values.data(),
        local_elements_count,
        MPI_INT,
        rank == 0 ? all_values.data() : nullptr,
        elements_per_process.data(),
        scatter_offsets.data(),
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start_time;

    if (rank == 0)
    {
        computation_start_time = MPI_Wtime();

        std::vector<int> sorted_data(total_elements);

        for (int value_index = 0; value_index < total_elements; value_index++)
        {
            sorted_data[all_ranks[value_index]] = all_values[value_index];
        }

        data = sorted_data;

        comp_time += MPI_Wtime() - computation_start_time;
    }
}