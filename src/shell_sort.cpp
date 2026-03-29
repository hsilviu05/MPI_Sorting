#include "../include/shell_sort.h"
#include <mpi.h>
#include <vector>
#include <algorithm>

void MPI_ShellSort(std::vector<int> &data, int rank, int size, double &comm_time, double &comp_time)
{
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
    ShellSortLocal(local_chunk);
    comp_time += MPI_Wtime() - computation_start_time;

    std::vector<int> gathered_chunks;
    if (rank == 0)
    {
        gathered_chunks.resize(total_elements);
    }

    communication_start_time = MPI_Wtime();
    MPI_Gather(
        local_chunk.data(),
        local_elements_count,
        MPI_INT,
        rank == 0 ? gathered_chunks.data() : nullptr,
        local_elements_count,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    comm_time += MPI_Wtime() - communication_start_time;

    if (rank == 0)
    {
        computation_start_time = MPI_Wtime();

        std::vector<int> merged_result(
            gathered_chunks.begin(),
            gathered_chunks.begin() + local_elements_count
        );

        for (int process_index = 1; process_index < size; process_index++)
        {
            const int chunk_start = process_index * local_elements_count;
            const int chunk_end = chunk_start + local_elements_count;

            std::vector<int> next_sorted_chunk(
                gathered_chunks.begin() + chunk_start,
                gathered_chunks.begin() + chunk_end
            );

            merged_result = MergeTwoSortedVectors(merged_result, next_sorted_chunk);
        }

        data = merged_result;

        comp_time += MPI_Wtime() - computation_start_time;
    }
}

void ShellSortLocal(std::vector<int> &values)
{
    for (int gap = static_cast<int>(values.size()) / 2; gap > 0; gap /= 2)
    {
        for (int current_index = gap; current_index < static_cast<int>(values.size()); current_index++)
        {
            const int current_value = values[current_index];
            int insertion_index = current_index;

            while (insertion_index >= gap && values[insertion_index - gap] > current_value)
            {
                values[insertion_index] = values[insertion_index - gap];
                insertion_index -= gap;
            }

            values[insertion_index] = current_value;
        }
    }
}

std::vector<int> MergeTwoSortedVectors(const std::vector<int> &left_values, const std::vector<int> &right_values)
{
    std::vector<int> merged_values;
    merged_values.reserve(left_values.size() + right_values.size());

    int left_index = 0;
    int right_index = 0;

    while (left_index < static_cast<int>(left_values.size()) && right_index < static_cast<int>(right_values.size()))
    {
        if (left_values[left_index] <= right_values[right_index])
        {
            merged_values.push_back(left_values[left_index]);
            left_index++;
        }
        else
        {
            merged_values.push_back(right_values[right_index]);
            right_index++;
        }
    }

    while (left_index < static_cast<int>(left_values.size()))
    {
        merged_values.push_back(left_values[left_index]);
        left_index++;
    }

    while (right_index < static_cast<int>(right_values.size()))
    {
        merged_values.push_back(right_values[right_index]);
        right_index++;
    }

    return merged_values;
}
