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

    int rank;
    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const std::vector<int> test_sizes = {100000, 500000, 1000000, 10000000, 50000000};

    if (rank == 0)
    {
        std::cout << "MPI Sorting\n";
        std::cout << "Numar procese: " << size << "\n";
    }

    for (const int array_size : test_sizes)
    {
        if (rank == 0)
        {
            std::cout << "\nDimensiune vector: " << array_size << "\n";
        }

        std::vector<int> original_data;

        if (rank == 0)
        {
            original_data = GenerateRandomArray(array_size);
        }

        // Bucket Sort
        {
            std::vector<int> data = original_data;

            double communication_time = 0.0;
            double computation_time = 0.0;

            MPI_Barrier(MPI_COMM_WORLD);
            const double total_start_time = MPI_Wtime();

            MPI_BucketSort(data, rank, size, communication_time, computation_time);

            MPI_Barrier(MPI_COMM_WORLD);
            const double total_end_time = MPI_Wtime();

            if (rank == 0)
            {
                const bool sorted_correctly = IsSorted(data);
                const double total_execution_time = total_end_time - total_start_time;

                std::cout << "[Bucket]   Total: " << total_execution_time
                          << " | Communication: " << communication_time
                          << " | Computation: " << computation_time
                          << " | Sorted: " << (sorted_correctly ? "Yes" : "No")
                          << "\n";
            }
        }

        // Odd-Even Sort
        
        {
            if (array_size % size != 0)
            {
                if (rank == 0)
                {
                    std::cout << "[Odd-Even] Dimensiunea vectorului nu este divizibila cu numarul de procese.\n";
                }
            }
            else
            {
                std::vector<int> data = original_data;

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
        }

        // Ranking Sort
        /*
        {
            std::vector<int> data = original_data;

            double communication_time = 0.0;
            double computation_time = 0.0;

            MPI_Barrier(MPI_COMM_WORLD);
            const double total_start_time = MPI_Wtime();

            MPI_RankingSort(data, rank, size, communication_time, computation_time);

            MPI_Barrier(MPI_COMM_WORLD);
            const double total_end_time = MPI_Wtime();

            if (rank == 0)
            {
                const bool sorted_correctly = IsSorted(data);
                const double total_execution_time = total_end_time - total_start_time;

                std::cout << "[Ranking]  Total: " << total_execution_time
                          << " | Communication: " << communication_time
                          << " | Computation: " << computation_time
                          << " | Sorted: " << (sorted_correctly ? "Yes" : "No")
                          << "\n";
            }
        }
        */

        // Shell Sort

        {
            std::vector<int> data = original_data;

            double communication_time = 0.0;
            double computation_time = 0.0;

            MPI_Barrier(MPI_COMM_WORLD);
            const double total_start_time = MPI_Wtime();

            MPI_ShellSort(data, rank, size, communication_time, computation_time);

            MPI_Barrier(MPI_COMM_WORLD);
            const double total_end_time = MPI_Wtime();

            if (rank == 0)
            {
                const bool sorted_correctly = IsSorted(data);
                const double total_execution_time = total_end_time - total_start_time;

                std::cout << "[Shell]    Total: " << total_execution_time
                          << " | Communication: " << communication_time
                          << " | Computation: " << computation_time
                          << " | Sorted: " << (sorted_correctly ? "Yes" : "No")
                          << "\n";
            }
        }

        // Bitonic Sort

        {
            std::vector<int> data = original_data;

            double communication_time = 0.0;
            double computation_time = 0.0;

            MPI_Barrier(MPI_COMM_WORLD);
            const double total_start_time = MPI_Wtime();

            MPI_BitonicSort(data, rank, size, communication_time, computation_time);

            MPI_Barrier(MPI_COMM_WORLD);
            const double total_end_time = MPI_Wtime();

            if (rank == 0)
            {
                const bool sorted_correctly = IsSorted(data);
                const double total_execution_time = total_end_time - total_start_time;

                std::cout << "[Bitonic]  Total: " << total_execution_time
                          << " | Communication: " << communication_time
                          << " | Computation: " << computation_time
                          << " | Sorted: " << (sorted_correctly ? "Yes" : "No")
                          << "\n";
            }
        }
    }

    MPI_Finalize();
    return 0;
}