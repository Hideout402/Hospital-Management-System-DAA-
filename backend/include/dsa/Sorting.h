#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include <functional>
using namespace std;

// Bubble Sort using Template
template <typename T, typename Compare = less<T>>
inline void bubbleSort(vector<T>& arr, Compare comp = Compare())
{
    for (int i = 0; i < arr.size(); i++)
    {
        for (int j = 0; j < arr.size() - i - 1; j++)
        {
            if (comp(arr[j + 1], arr[j]))
            {
                T temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Merge two parts
template <typename T, typename Compare>
inline void merge(vector<T>& arr, int left, int mid, int right, Compare comp)
{
    vector<T> temp;

    int i = left;
    int j = mid + 1;

    while (i <= mid && j <= right)
    {
        if (comp(arr[j], arr[i]))
            temp.push_back(arr[j++]);
        else
            temp.push_back(arr[i++]);
    }

    while (i <= mid)
        temp.push_back(arr[i++]);

    while (j <= right)
        temp.push_back(arr[j++]);

    for (int k = 0; k < temp.size(); k++)
        arr[left + k] = temp[k];
}

// Merge Sort using Template
template <typename T, typename Compare = less<T>>
inline void mergeSort(vector<T>& arr, int left, int right, Compare comp = Compare())
{
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

    mergeSort(arr, left, mid, comp);
    mergeSort(arr, mid + 1, right, comp);

    merge(arr, left, mid, right, comp);
}

#endif