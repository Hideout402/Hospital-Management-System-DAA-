#ifndef EAPPOINTMENTSORTING_H
#define EAPPOINTMENTSORTING_H

#include <functional>
#include <utility>
#include <vector>

namespace appointment_sorting_detail
{

template <typename T, typename Compare>
inline int partition(std::vector<T>& values, int low, int high, Compare comp)
{
    const T pivot = values[low + (high - low) / 2];
    int left = low - 1;
    int right = high + 1;

    while (true)
    {
        do
        {
            ++left;
        } while (comp(values[left], pivot));

        do
        {
            --right;
        } while (comp(pivot, values[right]));

        if (left >= right)
            return right;

        std::swap(values[left], values[right]);
    }
}

template <typename T, typename Compare>
inline void quickSortRange(std::vector<T>& values, int low, int high, Compare comp)
{
    if (low >= high)
        return;

    const int split = partition(values, low, high, comp);
    quickSortRange(values, low, split, comp);
    quickSortRange(values, split + 1, high, comp);
}

} // namespace appointment_sorting_detail

template <typename T, typename Compare = std::less<T>>
inline void quickSort(std::vector<T>& values, int low, int high, Compare comp = Compare())
{
    if (low < 0 || high >= static_cast<int>(values.size()))
        return;

    appointment_sorting_detail::quickSortRange(values, low, high, comp);
}

template <typename T, typename Compare = std::less<T>>
inline void quickSort(std::vector<T>& values, Compare comp = Compare())
{
    if (values.size() < 2)
        return;

    quickSort(values, 0, static_cast<int>(values.size()) - 1, comp);
}

#endif
