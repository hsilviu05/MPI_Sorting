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

    std::vector<int> test_sizes = {100000, 500000, 1000000};

    MPI_Finalize();
    return 0;
}