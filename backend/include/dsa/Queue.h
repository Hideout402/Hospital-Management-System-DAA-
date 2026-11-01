#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
using namespace std;

class SimpleQueue
{
private:
    queue<int> items;

public:
    void enqueue(int value)
    {
        items.push(value);
    }

    void dequeue()
    {
        if (!items.empty())
            items.pop();
    }

    int front()
    {
        if (items.empty())
            return -1;

        return items.front();
    }

    bool isEmpty()
    {
        return items.empty();
    }
};

#endif
