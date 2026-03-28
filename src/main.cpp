#include <mpi.h>
#include <iostream>
#include <vector>
#include <string>

#include "../include/utils.h"
#include "../include/ranking_sort.h"
#include "../include/bucket_sort.h"
#include "../include/odd_even_sort.h"
#include "../include/shell_sort.h"
#include "../include/bitonic_sort.h"

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const std::vector<int> test_sizes = {100000, 500000, 1000000, 10000000, 50000000};

    if (rank == 0)
    {
        std::cout << "MPI Odd-Even Sort\n";
        std::cout << "Numar procese: " << size << "\n";
    }

    for (const int array_size : test_sizes)
    {
        if (rank == 0)
        {
            std::cout << "Dimensiune vector: " << array_size << "\n";
        }

        if (array_size % size != 0)
        {
            if (rank == 0)
            {
                std::cout << "[Odd-Even] dimensiunea vectorului nu este divizibila cu numarul de procese.\n";
            }
            continue;
        }

        std::vector<int> data;

        if (rank == 0)
        {
            data = GenerateRandomArray(array_size);
        }

        double communication_time = 0.0;
        double computation_time = 0.0;

        MPI_Barrier(MPI_COMM_WORLD);
        const double total_start_time = MPI_Wtime();

        MPI_OddEvenSort(data, rank, size, communication_time, computation_time);

        MPI_Barrier(MPI_COMM_WORLD);
        const double total_end_time = MPI_Wtime();

        if (rank == 0)
        {
            const bool sorted_correctly = IsSorted(data);
            const double total_execution_time = total_end_time - total_start_time;

            std::cout << "[Odd-Even] Total: " << total_execution_time
                      << " | Communication: " << communication_time
                      << " | Computation: " << computation_time
                      << " | Sorted: " << (sorted_correctly ? "Yes" : "No")
                      << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}