#ifndef EAPPOINTMENTSTACK_H
#define EAPPOINTMENTSTACK_H

#include <string>
#include <vector>

struct EAppointmentAction
{
    int appointmentId;
    std::string action;
    std::string details;
    std::string timestamp;
};

class EAppointmentStack
{
private:
    struct Node
    {
        EAppointmentAction value;
        Node* next;

        explicit Node(const EAppointmentAction& action) : value(action), next(nullptr) {}
    };

    Node* topNode;
    int stackSize;

public:
    EAppointmentStack() : topNode(nullptr), stackSize(0) {}

    EAppointmentStack(const EAppointmentStack&) = delete;
    EAppointmentStack& operator=(const EAppointmentStack&) = delete;

    ~EAppointmentStack()
    {
        EAppointmentAction action{};
        while (pop(action)) {}
    }

    void push(const EAppointmentAction& action)
    {
        Node* node = new Node(action);
        node->next = topNode;
        topNode = node;
        ++stackSize;
    }

    bool pop(EAppointmentAction& action)
    {
        if (topNode == nullptr)
            return false;

        Node* removed = topNode;
        action = removed->value;
        topNode = topNode->next;
        delete removed;
        --stackSize;
        return true;
    }

    bool peek(EAppointmentAction& action) const
    {
        if (topNode == nullptr)
            return false;
        action = topNode->value;
        return true;
    }

    bool isEmpty() const
    {
        return topNode == nullptr;
    }

    int size() const
    {
        return stackSize;
    }

    std::vector<EAppointmentAction> toVector() const
    {
        std::vector<EAppointmentAction> result;
        result.reserve(static_cast<size_t>(stackSize));
        for (Node* current = topNode; current != nullptr; current = current->next)
            result.push_back(current->value);
        return result;
    }
};

#endif
