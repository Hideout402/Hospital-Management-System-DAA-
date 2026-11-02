#ifndef DOCTORLIST_H
#define DOCTORLIST_H

#include <vector>
#include <string>
#include <algorithm>
#include "../models/Doctor.h"

struct DoctorNode {
    Doctor data;
    DoctorNode* prev = nullptr;
    DoctorNode* next = nullptr;

    explicit DoctorNode(const Doctor& doc)
        : data(doc), prev(nullptr), next(nullptr) {}
};

class DoctorDoublyLinkedList {
private:
    DoctorNode* head = nullptr;
    DoctorNode* tail = nullptr;
    int nodeCount = 0;

public:
    DoctorDoublyLinkedList() = default;

    ~DoctorDoublyLinkedList() {
        clear();
    }

    DoctorDoublyLinkedList(const DoctorDoublyLinkedList&) = delete;
    DoctorDoublyLinkedList& operator=(const DoctorDoublyLinkedList&) = delete;

    void clear() {
        DoctorNode* current = head;
        while (current != nullptr) {
            DoctorNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
        tail = nullptr;
        nodeCount = 0;
    }

    int size() const {
        return nodeCount;
    }

    bool empty() const {
        return nodeCount == 0;
    }

    // Insert at tail of roster (O(1))
    void push_back(const Doctor& doc) {
        auto* newNode = new DoctorNode(doc);
        if (!tail) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        nodeCount++;
    }

    // Insert at head of roster (O(1))
    void push_front(const Doctor& doc) {
        auto* newNode = new DoctorNode(doc);
        if (!head) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        nodeCount++;
    }

    // Build roster from vector of doctors
    void buildFromVector(const std::vector<Doctor>& doctors) {
        clear();
        for (const auto& doc : doctors) {
            push_back(doc);
        }
    }

    // Find node pointer
    DoctorNode* findNode(int id) const {
        DoctorNode* curr = head;
        while (curr != nullptr) {
            if (curr->data.id == id) {
                return curr;
            }
            curr = curr->next;
        }
        return nullptr;
    }

    // Forward search (head -> tail)
    Doctor* findForward(int id) const {
        DoctorNode* curr = head;
        while (curr != nullptr) {
            if (curr->data.id == id) {
                return &(curr->data);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    // Backward search (tail -> head)
    Doctor* findBackward(int id) const {
        DoctorNode* curr = tail;
        while (curr != nullptr) {
            if (curr->data.id == id) {
                return &(curr->data);
            }
            curr = curr->prev;
        }
        return nullptr;
    }

    // Remove doctor from roster by ID (O(1) relinking)
    bool remove(int id) {
        DoctorNode* target = findNode(id);
        if (!target) return false;

        if (target->prev) {
            target->prev->next = target->next;
        } else {
            head = target->next; // target was head
        }

        if (target->next) {
            target->next->prev = target->prev;
        } else {
            tail = target->prev; // target was tail
        }

        delete target;
        nodeCount--;
        return true;
    }

    // Update doctor in place
    bool update(int id, const Doctor& updatedDoc) {
        DoctorNode* target = findNode(id);
        if (!target) return false;
        target->data = updatedDoc;
        return true;
    }

    // Bi-directional Traversal: Forward (head to tail)
    std::vector<Doctor> toVectorForward() const {
        std::vector<Doctor> result;
        result.reserve(nodeCount);
        DoctorNode* curr = head;
        while (curr != nullptr) {
            result.push_back(curr->data);
            curr = curr->next;
        }
        return result;
    }

    // Bi-directional Traversal: Backward (tail to head)
    std::vector<Doctor> toVectorBackward() const {
        std::vector<Doctor> result;
        result.reserve(nodeCount);
        DoctorNode* curr = tail;
        while (curr != nullptr) {
            result.push_back(curr->data);
            curr = curr->prev;
        }
        return result;
    }

    // Next doctor in roster (supports circular wrap-around)
    const Doctor* getNextDoctor(int currentId, bool wrapAround = true) const {
        DoctorNode* curr = findNode(currentId);
        if (!curr) return nullptr;

        if (curr->next) {
            return &(curr->next->data);
        } else if (wrapAround && head) {
            return &(head->data);
        }
        return nullptr;
    }

    // Previous doctor in roster (supports circular wrap-around)
    const Doctor* getPrevDoctor(int currentId, bool wrapAround = true) const {
        DoctorNode* curr = findNode(currentId);
        if (!curr) return nullptr;

        if (curr->prev) {
            return &(curr->prev->data);
        } else if (wrapAround && tail) {
            return &(tail->data);
        }
        return nullptr;
    }

    // Filter active doctors using bi-directional traversal
    std::vector<Doctor> filterByAvailability(const std::string& status, bool forward = true) const {
        std::vector<Doctor> result;
        DoctorNode* curr = forward ? head : tail;
        while (curr != nullptr) {
            if (status.empty() || curr->data.availability == status) {
                result.push_back(curr->data);
            }
            curr = forward ? curr->next : curr->prev;
        }
        return result;
    }
};

#endif
