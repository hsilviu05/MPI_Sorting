#pragma once
#include <vector>
#include <string>

std::vector<int> generate_random_array(int n, int min_val = 0, int max_val = 1000000);
bool is_sorted_array(const std::vector<int>& arr);
void print_array(const std::vector<int>& arr, const std::string& label);

