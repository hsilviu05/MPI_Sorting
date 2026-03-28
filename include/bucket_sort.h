#pragma once
#include <vector>

void MPI_BucketSort(std::vector<int>& data, int rank, int size, double& comm_time, double& comp_time);
