#pragma once
#include <vector>

void MPI_RankingSort(std::vector<int>& data, int rank, int size, double& comm_time, double& comp_time);

