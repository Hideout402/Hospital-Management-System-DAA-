#ifndef LINKEDLISTS_H
#define LINKEDLISTS_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Node
{
    string data;
    Node* next;
};

class SinglyLinkedList
{
private:
    Node* head;

public:
    SinglyLinkedList()
    {
        head = nullptr;
    }

    void insert(string value)
    {
        Node* newNode = new Node;
        newNode->data = value;
        newNode->next = nullptr;

        if (head == nullptr)
        {
            head = newNode;
            return;
        }

        Node* temp = head;

        while (temp->next != nullptr)
        {
            temp = temp->next;
        }

        temp->next = newNode;
    }

    vector<string> toVector() const
    {
        vector<string> values;
        Node* temp = head;

        while (temp != nullptr)
        {
            values.push_back(temp->data);
            temp = temp->next;
        }

        return values;
    }

    void display()
    {
        Node* temp = head;

        while (temp != nullptr)
        {
            cout << temp->data << " -> ";
            temp = temp->next;
        }

        cout << "NULL" << endl;
    }

    ~SinglyLinkedList()
    {
        Node* temp;

        while (head != nullptr)
        {
            temp = head;
            head = head->next;
            delete temp;
        }
    }
};

#endif