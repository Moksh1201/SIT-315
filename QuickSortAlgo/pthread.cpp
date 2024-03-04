#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <pthread.h>
#include <algorithm> 

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
        // For Creating thread for smaller partition
        pthread_t thread;
        struct ThreadArgs args;
        args.arr = &arr;
        args.left = left;
        args.right = j;
        pthread_create(&thread, NULL, threadedQuickSort, (void*)&args);
        
        // Sort the larger partition in the current thread
        quickSort(arr, i, right);
        
        // Wait for the thread to finish
        pthread_join(thread, NULL);
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
