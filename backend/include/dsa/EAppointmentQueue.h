#ifndef EAPPOINTMENTQUEUE_H
#define EAPPOINTMENTQUEUE_H

#include <vector>

class EAppointmentQueue
{
private:
    struct Node
    {
        int appointmentId;
        Node* next;

        explicit Node(int id) : appointmentId(id), next(nullptr) {}
    };

    Node* frontNode;
    Node* rearNode;
    int queueSize;

public:
    EAppointmentQueue() : frontNode(nullptr), rearNode(nullptr), queueSize(0) {}

    EAppointmentQueue(const EAppointmentQueue&) = delete;
    EAppointmentQueue& operator=(const EAppointmentQueue&) = delete;

    ~EAppointmentQueue()
    {
        while (!isEmpty())
            dequeue();
    }

    bool enqueue(int appointmentId)
    {
        Node* node = new Node(appointmentId);
        if (rearNode == nullptr)
            frontNode = rearNode = node;
        else
        {
            rearNode->next = node;
            rearNode = node;
        }
        ++queueSize;
        return true;
    }

    int dequeue()
    {
        if (frontNode == nullptr)
            return -1;

        Node* removed = frontNode;
        const int appointmentId = removed->appointmentId;
        frontNode = frontNode->next;
        if (frontNode == nullptr)
            rearNode = nullptr;
        delete removed;
        --queueSize;
        return appointmentId;
    }

    int front() const
    {
        return frontNode == nullptr ? -1 : frontNode->appointmentId;
    }

    bool contains(int appointmentId) const
    {
        for (Node* current = frontNode; current != nullptr; current = current->next)
        {
            if (current->appointmentId == appointmentId)
                return true;
        }
        return false;
    }

    bool remove(int appointmentId)
    {
        Node* previous = nullptr;
        Node* current = frontNode;
        while (current != nullptr && current->appointmentId != appointmentId)
        {
            previous = current;
            current = current->next;
        }
        if (current == nullptr)
            return false;

        if (previous == nullptr)
            frontNode = current->next;
        else
            previous->next = current->next;
        if (rearNode == current)
            rearNode = previous;
        delete current;
        --queueSize;
        return true;
    }

    bool isEmpty() const
    {
        return frontNode == nullptr;
    }

    int size() const
    {
        return queueSize;
    }

    std::vector<int> toVector() const
    {
        std::vector<int> result;
        result.reserve(static_cast<size_t>(queueSize));
        for (Node* current = frontNode; current != nullptr; current = current->next)
            result.push_back(current->appointmentId);
        return result;
    }
};

#endif
