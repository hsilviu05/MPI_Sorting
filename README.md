# MPI Sorting Assignment

Parallel sorting algorithms implemented with MPI in C++ for the PPDC assignment.

This project implements and evaluates the following MPI-based sorting algorithms:

- Ranking Sort
- Bucket Sort
- Odd-Even Sort
- Shell Sort
- Bitonic Sort

The goal of the project is to compare the performance of these algorithms on large datasets and analyze their behavior in terms of execution time, communication time, computation time, speedup, and overhead.

---

## 1. Assignment Objective

This assignment aims to develop some of the sorting algorithms covered in the lectures and evaluate their performance in a parallel MPI environment.

### Required tasks

- Implement MPI versions of:
  - Ranking Sort
  - Bucket Sort
  - Odd-Even Sort
  - Shell Sort
  - Bitonic Sort
- Sort arrays with the following input sizes:
  - 100000
  - 500000
  - 1000000
  - 10000000
  - 50000000
- Verify correctness using `is_sorted`
- Evaluate:
  - execution times
  - communication times
  - computation times
  - speedup
  - overhead
- Discuss the observed performance in a report

---

## 2. Project Structure

```text
sorting/
├── include/
│   ├── utils.h
│   ├── ranking_sort.h
│   ├── bucket_sort.h
│   ├── odd_even_sort.h
│   ├── shell_sort.h
│   └── bitonic_sort.h
│
├── src/
│   ├── main.cpp
│   ├── utils.cpp
│   ├── ranking_sort.cpp
│   ├── bucket_sort.cpp
│   ├── odd_even_sort.cpp
│   ├── shell_sort.cpp
│   └── bitonic_sort.cpp
│
├── docs/
│   ├── MPI_Sorting_Evaluation_Report.pdf
│   └── MPI_Sorting_Analysis_with_Graphs.xlsx
│
├── graphs/
│   ├── ranking_performance.png
│   ├── bucket_performance.png
│   ├── odd_even_performance.png
│   ├── shell_performance.png
│   └── bitonic_performance.png
│
├── .vscode/
│   ├── tasks.json
│   └── c_cpp_properties.json
│
├── .gitignore
└── README.md