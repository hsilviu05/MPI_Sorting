#include <vector>

void MPI_BitonicSort(std::vector<int>& data, int rank, int size, double& comm_time, double& comp_time);

bool IsPowerOfTwo(int value);
void KeepSmallerHalf(std::vector<int>& local_chunk, const std::vector<int>& partner_chunk);
void KeepLargerHalf(std::vector<int>& local_chunk, const std::vector<int>& partner_chunk);