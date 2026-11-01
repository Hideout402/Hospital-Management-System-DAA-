#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <queue>
using namespace std;

class EmergencyPriorityQueue
{
private:
    priority_queue<int, vector<int>, greater<int>> patients;

public:
    void addPatient(int triageLevel)
    {
        patients.push(triageLevel);
    }

    int getNextPatient()
    {
        if (patients.empty())
            return -1;

        int priority = patients.top();
        patients.pop();

        return priority;
    }

    bool isEmpty()
    {
        return patients.empty();
    }
};

#endif
