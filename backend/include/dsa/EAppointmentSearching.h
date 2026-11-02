#ifndef EAPPOINTMENTSEARCHING_H
#define EAPPOINTMENTSEARCHING_H

#include <functional>
#include <vector>

template <typename T, typename Key, typename KeyGetter, typename Compare = std::less<Key>>
inline int appointmentBinarySearch(
    const std::vector<T>& values,
    const Key& target,
    KeyGetter getKey,
    Compare comp = Compare())
{
    int low = 0;
    int high = static_cast<int>(values.size()) - 1;

    while (low <= high)
    {
        const int middle = low + (high - low) / 2;
        const Key current = getKey(values[middle]);

        if (!comp(current, target) && !comp(target, current))
            return middle;
        if (comp(current, target))
            low = middle + 1;
        else
            high = middle - 1;
    }

    return -1;
}

#endif
