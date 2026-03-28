#include "../include/utils.h"
#include <iostream>
#include <random>

std::vector<int> generate_random_array(int n, int min_val, int max_val) {
    std::vector<int> arr(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min_val, max_val);

    for (int i = 0; i < n; i++) {
        arr[i] = dist(gen);
    }

    return arr;
}

bool is_sorted_array(const std::vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }
    return true;
}

void print_array(const std::vector<int>& arr, const std::string& label) {
    std::cout << label << ": ";
    for (int x : arr) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}