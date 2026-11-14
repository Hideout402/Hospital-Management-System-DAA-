#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <vector>
#include "../models/EmergencyCase.h"
using namespace std;

// Manual min-heap stored in an array.
// The case with the smallest triageLevel (1 = most critical) is always at the top.
// If two cases have the same triageLevel, the smaller arrivalOrder comes first.
class EmergencyPriorityQueue
{
private:
    vector<EmergencyCase> heap;

    // true if a must come out before b
    bool comesBefore(const EmergencyCase& a, const EmergencyCase& b) const
    {
        if (a.triageLevel != b.triageLevel)
            return a.triageLevel < b.triageLevel;
        return a.arrivalOrder < b.arrivalOrder;
    }

    void swapCases(int i, int j)
    {
        EmergencyCase temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    // Move a new item up until its parent is smaller
    void heapifyUp(int index)
    {
        while (index > 0)
        {
            int parent = (index - 1) / 2;
            if (comesBefore(heap[index], heap[parent]))
            {
                swapCases(index, parent);
                index = parent;
            }
            else
            {
                break;
            }
        }
    }

    // Move an item down until both children are bigger
    void heapifyDown(int index)
    {
        int size = (int)heap.size();

        while (true)
        {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;

            if (left < size && comesBefore(heap[left], heap[smallest]))
                smallest = left;
            if (right < size && comesBefore(heap[right], heap[smallest]))
                smallest = right;

            if (smallest == index)
                break;

            swapCases(index, smallest);
            index = smallest;
        }
    }

public:
    // O(log n)
    void insert(const EmergencyCase& c)
    {
        heap.push_back(c);
        heapifyUp((int)heap.size() - 1);
    }

    // O(1): look at the most urgent case without removing it
    bool peek(EmergencyCase& result) const
    {
        if (heap.empty())
            return false;
        result = heap[0];
        return true;
    }

    // O(log n): remove and return the most urgent case
    bool extractMin(EmergencyCase& result)
    {
        if (heap.empty())
            return false;

        result = heap[0];
        heap[0] = heap[heap.size() - 1];
        heap.pop_back();

        if (!heap.empty())
            heapifyDown(0);

        return true;
    }

    bool isEmpty() const { return heap.empty(); }
    int size() const { return (int)heap.size(); }
};

#endif