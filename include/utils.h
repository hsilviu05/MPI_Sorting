#pragma once
#include <vector>
#include <string>

std::vector<int> GenerateRandomArray(int n, int min_val = 0, int max_val = 1000000);
bool IsSorted(const std::vector<int>& arr);
void PrintArray(const std::vector<int>& arr, const std::string& label);

