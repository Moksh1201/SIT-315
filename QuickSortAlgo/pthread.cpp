#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <pthread.h>
#include <algorithm> // Required for std::sort

using namespace std;
using namespace std::chrono;

// Structure for passing arguments to threads
struct ThreadArgs {
    vector<int>* arr;
    int left;
    int right;
};

// Function declarations
void quickSort(vector<int>& arr, int left, int right);
void* threadedQuickSort(void* args);
vector<int> generateRandomData(int size);

// Main quicksort function
void quickSort(vector<int>& arr, int left, int right) {
    // If left index is greater than or equal to right index, return
    if (left >= right) return;

    int i = left, j = right;
    int pivot = arr[(left + right) / 2]; // Select pivot element

    // Partitioning step
    while (i <= j) {
        while (arr[i] < pivot) i++; // Move the left pointer
        while (arr[j] > pivot) j--; // Move the right pointer
        if (i <= j) {
            swap(arr[i], arr[j]); // Swap elements if necessary
            i++;
            j--;
        }
    };

    // Recursion step
    if (right - left < 1000) { // Use sequential sort for small subarrays
        sort(arr.begin() + left, arr.begin() + right + 1);
    } else {
        // Load balancing: process larger partition in current thread
        if (j - left > right - i) {
            quickSort(arr, left, j); // Recursively sort left partition
            // Create thread for smaller partition
            pthread_t rightThread;
            struct ThreadArgs rightArgs;
            rightArgs.arr = &arr;
            rightArgs.left = i;
            rightArgs.right = right;
            pthread_create(&rightThread, NULL, threadedQuickSort, (void*)&rightArgs);
            // Wait for thread to finish
            pthread_join(rightThread, NULL);
        } else {
            quickSort(arr, i, right); // Recursively sort right partition
            // Create thread for smaller partition
            pthread_t leftThread;
            struct ThreadArgs leftArgs;
            leftArgs.arr = &arr;
            leftArgs.left = left;
            leftArgs.right = j;
            pthread_create(&leftThread, NULL, threadedQuickSort, (void*)&leftArgs);
            // Wait for thread to finish
            pthread_join(leftThread, NULL);
        }
    }
}

// Threaded quicksort function
void* threadedQuickSort(void* args) {
    struct ThreadArgs* targs = (struct ThreadArgs*)args;
    quickSort(*(targs->arr), targs->left, targs->right);
    return nullptr;
}

// Function to generate random data
vector<int> generateRandomData(int size) {
    vector<int> data;
    for (int i = 0; i < size; ++i) {
        data.push_back(rand() % 1000); // Generate random numbers in the range [0, 999]
    }
    return data;
}

int main() {
    // Seed random number generator
    srand(time(0));

    // Generate random data
    int dataSize = 10000;
    vector<int> data = generateRandomData(dataSize);

    // Measure execution time
    auto start = high_resolution_clock::now();
    quickSort(data, 0, data.size() - 1);
    auto stop = high_resolution_clock::now();

    // Calculate execution time in microseconds
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Execution time: " << duration.count() << " microseconds" << endl;

    return 0;
}
