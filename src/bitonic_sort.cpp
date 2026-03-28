#include "../include/bitonic_sort.h"
#include <mpi.h>
#include <vector>
#include <algorithm>

void MPI_BitonicSort(std::vector<int> &data, int rank, int size, double &comm_time, double &comp_time)
{
     if (!IsPowerOfTwo(size))
    {
        return;
    }

    int total_elements;

    if (rank == 0)
    {
        total_elements = static_cast<int>(data.size());
    }

    double communication_start_time = MPI_Wtime();
    MPI_Bcast(&total_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);
    comm_time += MPI_Wtime() - communication_start_time;

    if (total_elements % size != 0)
    {
        return;
    }

    const int local_elements_count = total_elements / size;
    std::vector<int> local_chunk(local_elements_count);

    communication_start_time = MPI_Wtime();
    MPI_Scatter(
        rank == 0 ? data.data() : nullptr,
        local_elements_count,
        MPI_INT,
        local_chunk.data(),
        local_elements_count,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start_time;

    double computation_start_time = MPI_Wtime();
    std::sort(local_chunk.begin(), local_chunk.end());
    comp_time += MPI_Wtime() - computation_start_time;

    std::vector<int> partner_chunk(local_elements_count);

    for (int sequence_size = 2; sequence_size <= size; sequence_size *= 2)
    {
        for (int comparison_distance = sequence_size / 2; comparison_distance > 0; comparison_distance /= 2)
        {
            const int partner_rank = rank ^ comparison_distance;

            communication_start_time = MPI_Wtime();
            MPI_Sendrecv(
                local_chunk.data(),
                local_elements_count,
                MPI_INT,
                partner_rank,
                0,
                partner_chunk.data(),
                local_elements_count,
                MPI_INT,
                partner_rank,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
            );
            comm_time += MPI_Wtime() - communication_start_time;

            const bool sort_in_ascending_order = ((rank & sequence_size) == 0);

            computation_start_time = MPI_Wtime();

            if (sort_in_ascending_order)
            {
                if ((rank & comparison_distance) == 0)
                {
                    KeepSmallerHalf(local_chunk, partner_chunk);
                }
                else
                {
                    KeepLargerHalf(local_chunk, partner_chunk);
                }
            }
            else
            {
                if ((rank & comparison_distance) == 0)
                {
                    KeepLargerHalf(local_chunk, partner_chunk);
                }
                else
                {
                    KeepSmallerHalf(local_chunk, partner_chunk);
                }
            }

            comp_time += MPI_Wtime() - computation_start_time;
        }
    }

    communication_start_time = MPI_Wtime();
    MPI_Gather(
        local_chunk.data(),
        local_elements_count,
        MPI_INT,
        rank == 0 ? data.data() : nullptr,
        local_elements_count,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start_time;
}

bool IsPowerOfTwo(int value)
{
    return value > 0 && (value & (value - 1)) == 0;
}

void KeepSmallerHalf(std::vector<int> &local_chunk, const std::vector<int> &partner_chunk)
{
    std::vector<int> merged_values;
    merged_values.reserve(local_chunk.size() + partner_chunk.size());

    std::merge(
        local_chunk.begin(), local_chunk.end(),
        partner_chunk.begin(), partner_chunk.end(),
        std::back_inserter(merged_values));

    local_chunk.assign(merged_values.begin(), merged_values.begin() + local_chunk.size());
}

void KeepLargerHalf(std::vector<int> &local_chunk, const std::vector<int> &partner_chunk)
{
    std::vector<int> merged_values;
    merged_values.reserve(local_chunk.size() + partner_chunk.size());

    std::merge(
        local_chunk.begin(), local_chunk.end(),
        partner_chunk.begin(), partner_chunk.end(),
        std::back_inserter(merged_values));

    local_chunk.assign(merged_values.end() - local_chunk.size(), merged_values.end());
}
