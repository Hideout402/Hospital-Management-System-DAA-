#ifndef STACK_H
#define STACK_H

#include <vector>
using namespace std;

class SimpleStack
{
private:
    vector<int> items;

public:
    void push(int value)
    {
        items.push_back(value);
    }

    void pop()
    {
        if (!items.empty())
            items.pop_back();
    }

    int top()
    {
        if (items.empty())
            return -1;

        return items.back();
    }

    bool isEmpty()
    {
        return items.empty();
    }
};

#endif
