
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
using namespace std;

class SimpleHashTable
{
private:
    int table[10];

public:
    SimpleHashTable()
    {
        for (int i = 0; i < 10; i++)
            table[i] = -1;
    }

    bool insert(int value)
    {
        int index = value % 10;

        for (int i = 0; i < 10; i++)
        {
            if (table[index] == value)
                return false;

            if (table[index] == -1)
            {
                table[index] = value;
                return true;
            }

            index = (index + 1) % 10;
        }

        return false;
    }

    bool search(int value)
    {
        int index = value % 10;

        for (int i = 0; i < 10; i++)
        {
            if (table[index] == -1)
                return false;

            if (table[index] == value)
                return true;

            index = (index + 1) % 10;
        }

        return false;
    }
};

#endif
