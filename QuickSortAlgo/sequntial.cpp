#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using namespace std::chrono;

// This is the quicksort function. It sorts a given array from index 'left' to index 'right'.
void quickSort(vector<int>& arr, int left, int right) {
    int i = left, j = right;
    int pivot = arr[(left + right) / 2]; // Selecting the pivot element.

    // Partitioning step
    while (i <= j) {
        while (arr[i] < pivot) i++; // Move the left pointer to the right until it finds an element greater than or equal to the pivot.
        while (arr[j] > pivot) j--; // Move the right pointer to the left until it finds an element less than or equal to the pivot.
        if (i <= j) { // If the pointers have not crossed each other.
            swap(arr[i], arr[j]); // Swap the elements at the positions of the pointers.
            i++; // Move the left pointer to the right.
            j--; // Move the right pointer to the left.
        }
    };

    // Recursion
    if (left < j) // Recursively sort the left partition if there are elements to the left of the pivot.
        quickSort(arr, left, j);
    if (i < right) // Recursively sort the right partition if there are elements to the right of the pivot.
        quickSort(arr, i, right);
}

// This function generates random data of a given size and returns it as a vector.
vector<int> generateRandomData(int size) {
    vector<int> data;
    for (int i = 0; i < size; ++i) {
        data.push_back(rand() % 1000); // Generate random numbers in the range [0, 999].
    }
    return data;
}

int main() {
    srand(time(0)); // Seed the random number generator with the current time.

    int dataSize = 10000; // Define the size of the data to be generated.
    vector<int> data = generateRandomData(dataSize); // Generate random data of the specified size.

    auto start = high_resolution_clock::now(); // Record the start time.

    quickSort(data, 0, data.size() - 1); // Call the quickSort function to sort the generated data.

    auto stop = high_resolution_clock::now(); // Record the stop time.

    auto duration = duration_cast<microseconds>(stop - start); // Calculate the execution time in microseconds.

    cout << "Execution time: " << duration.count() << " microseconds" << endl; // Print the execution time.

    return 0;
}
