# Parallel-and-Distributed-Computing-Assignment-1-Pthreads

1. Serial Version: Students will implement a program to
i. read a list of numbers from file and store them in an array,
ii. insert the individual numbers into a linked list,
iii. and sort the linked list using merge sort
Serial version functions
void readRollNumbers(FILE* inputFile, int* Numbers, int num);
void addRollNumbersToList(Node** head, int* Numbers, int num);
Node* mergeSort(Node* head);
2. Parallel Version with CPU Affinity:
o Create a pthread function to add roll numbers into the linked list concurrently.
Each thread will handle a subset of the roll numbers.
o Implement a pthread function to parallelize the merge sort algorithm on the
linked list. Threads will work on different portions of the list during the merge
sort process.

o incorporate pthread_setaffinity_np for setting CPU affinity on a per-
thread basis. Each thread in the parallel version will be mapped to a specific

CPU core using pthread_setaffinity_np
i. plan and implement the mapping of threads to specific CPU cores
using the CPU affinity concept, exploiting sched_setaffinity
function calls.
ii. Utilize pthread_setaffinity_np to set CPU affinity for each thread,
specifying the CPU cores they should be bound to. Details related to

affinity available here ( https://man7.org/linux/man-
pages/man3/pthread_setaffinity_np.3.html ).

#include <pthread.h>
// Parallel version functions with pthread_setaffinity_np
void* addRollNumbersToListParallel(void* arg);
void* mergeSortParallel(void* arg);
void setAffinity(pthread_t thread, int coreId);


Additional Requirements
1. Thread Mapping Plan:
o Before implementing the parallel version, students should analyze the system's
architecture and come up with a plan for mapping threads to specific CPU cores.
o Consider factors such as the number of available cores, workload distribution,
and potential resource contention.
o Document the chosen mapping strategy and justify why it is suitable for the
given scenario.
2. Testing and Analysis:
o Measure the execution time of both serial and parallel versions for varying input
sizes.
o Analyze and compare the performance of the serial and parallel versions with
and without CPU affinity.
o Discuss any speedup, and possible bottlenecks introduced through CPU affinity.
General speedup formulas are provided below you can use them as per your
requirements.
