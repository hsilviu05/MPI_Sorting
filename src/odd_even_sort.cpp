#include "../include/odd_even_sort.h"
#include <mpi.h>
#include <vector>
#include <algorithm>
#include <iostream>

void MPI_OddEvenSort(std::vector<int>& data, int rank, int size, double& comm_time, double& comp_time)
{
    int global_array_size = 0;

    if (rank == 0)
    {
        global_array_size = static_cast<int>(data.size());
    }

    double communication_start_time = MPI_Wtime();
    MPI_Bcast(&global_array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    comm_time += MPI_Wtime() - communication_start_time;

    if (global_array_size % size != 0)
    {
        if (rank == 0)
        {
            std::cerr << "Eroare: dimensiunea vectorului trebuie sa fie divizibila cu numarul de procese pentru aceasta implementare.\n";
        }
        return;
    }

    const int local_segment_size = global_array_size / size;
    std::vector<int> local_segment(local_segment_size);

    communication_start_time = MPI_Wtime();
    MPI_Scatter(
        rank == 0 ? data.data() : nullptr,
        local_segment_size,
        MPI_INT,
        local_segment.data(),
        local_segment_size,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start_time;

    double computation_start_time = MPI_Wtime();
    std::sort(local_segment.begin(), local_segment.end());
    comp_time += MPI_Wtime() - computation_start_time;

    for (int phase = 0; phase < size; phase++)
    {
        int partner_rank = -1;

        if (phase % 2 == 0)
        {
            if (rank % 2 == 0)
            {
                partner_rank = rank + 1;
            }
            else
            {
                partner_rank = rank - 1;
            }
        }
        else
        {
            if (rank % 2 == 0)
            {
                partner_rank = rank - 1;
            }
            else
            {
                partner_rank = rank + 1;
            }
        }

        if (partner_rank < 0 || partner_rank >= size)
        {
            continue;
        }

        std::vector<int> received_segment(local_segment_size);

        communication_start_time = MPI_Wtime();
        MPI_Sendrecv(
            local_segment.data(),
            local_segment_size,
            MPI_INT,
            partner_rank,
            0,
            received_segment.data(),
            local_segment_size,
            MPI_INT,
            partner_rank,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
        comm_time += MPI_Wtime() - communication_start_time;

        computation_start_time = MPI_Wtime();
        if (rank < partner_rank)
        {
            merge_keep_small(local_segment, received_segment);
        }
        else
        {
            merge_keep_large(local_segment, received_segment);
        }
        comp_time += MPI_Wtime() - computation_start_time;
    }

    communication_start_time = MPI_Wtime();
    MPI_Gather(
        local_segment.data(),
        local_segment_size,
        MPI_INT,
        rank == 0 ? data.data() : nullptr,
        local_segment_size,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start_time;
}

static void merge_keep_small(std::vector<int>& local_segment, const std::vector<int>& received_segment)
{
    std::vector<int> merged_segments(local_segment.size() + received_segment.size());

    std::merge(
        local_segment.begin(), local_segment.end(),
        received_segment.begin(), received_segment.end(),
        merged_segments.begin()
    );

    for (size_t index = 0; index < local_segment.size(); index++)
    {
        local_segment[index] = merged_segments[index];
    }
}

static void merge_keep_large(std::vector<int>& local_segment, const std::vector<int>& received_segment)
{
    std::vector<int> merged_segments(local_segment.size() + received_segment.size());

    std::merge(
        local_segment.begin(), local_segment.end(),
        received_segment.begin(), received_segment.end(),
        merged_segments.begin()
    );

    const size_t local_segment_size = local_segment.size();

    for (size_t index = 0; index < local_segment_size; index++)
    {
        local_segment[index] = merged_segments[merged_segments.size() - local_segment_size + index];
    }
}
