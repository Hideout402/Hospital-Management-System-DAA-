#ifndef SEARCHING_H
#define SEARCHING_H

#include <vector>
using namespace std;

// Linear Search using Template
template <typename T>
inline int linearSearch(const vector<T>& arr, const T& target)
{
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] == target)
            return i;
    }

    return -1;
}

// Binary Search using Template
template <typename T>
inline int binarySearch(const vector<T>& arr, const T& target)
{
    int low = 0;
    int high = static_cast<int>(arr.size()) - 1;

    while (low <= high)
    {
        int mid = low + (high - low) / 2;

        if (arr[mid] == target)
            return mid;

        if (arr[mid] < target)
            low = mid + 1;
        else
            high = mid - 1;
    }

    return -1;
}

#endif