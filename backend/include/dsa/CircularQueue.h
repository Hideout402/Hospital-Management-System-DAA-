#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

class CircularQueue
{
private:
    int arr[10];
    int frontIndex;
    int rearIndex;
    int count;

public:
    CircularQueue()
    {
        frontIndex = 0;
        rearIndex = -1;
        count = 0;
    }

    bool enqueue(int value)
    {
        if (count == 10)
            return false;

        rearIndex = (rearIndex + 1) % 10;
        arr[rearIndex] = value;
        count++;

        return true;
    }

    bool dequeue()
    {
        if (count == 0)
            return false;

        frontIndex = (frontIndex + 1) % 10;
        count--;

        return true;
    }

    int front()
    {
        if (count == 0)
            return -1;

        return arr[frontIndex];
    }

    bool isEmpty()
    {
        return count == 0;
    }
};

#endif
