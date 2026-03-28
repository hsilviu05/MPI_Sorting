#pragma once
#include <vector>

void MPI_OddEvenSort(std::vector<int>& data, int rank, int size, double& comm_time, double& comp_time);

static void merge_keep_small(std::vector<int>& local, const std::vector<int>& recv);
static void merge_keep_large(std::vector<int>& local, const std::vector<int>& recv);