#include <vector>

void MPI_ShellSort(std::vector<int>& data, int rank, int size, double& comm_time, double& comp_time);

void ShellSortLocal(std::vector<int> &values);
std::vector<int> MergeTwoSortedVectors(const std::vector<int> &left_values, const std::vector<int> &right_values);
