#ifndef DOCTORBST_H
#define DOCTORBST_H

#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include "../models/Doctor.h"

struct DoctorBSTNode {
    Doctor data;
    DoctorBSTNode* left = nullptr;
    DoctorBSTNode* right = nullptr;

    explicit DoctorBSTNode(const Doctor& doc)
        : data(doc), left(nullptr), right(nullptr) {}
};

class DoctorBST {
private:
    DoctorBSTNode* root = nullptr;

    static std::string toLower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return str;
    }

    void destroyRecursive(DoctorBSTNode* node) {
        if (!node) return;
        destroyRecursive(node->left);
        destroyRecursive(node->right);
        delete node;
    }

    DoctorBSTNode* insertRecursive(DoctorBSTNode* node, const Doctor& doc) {
        if (!node) {
            return new DoctorBSTNode(doc);
        }
        if (doc.id < node->data.id) {
            node->left = insertRecursive(node->left, doc);
        } else if (doc.id > node->data.id) {
            node->right = insertRecursive(node->right, doc);
        } else {
            // Update in place if ID matches
            node->data = doc;
        }
        return node;
    }

    DoctorBSTNode* searchRecursive(DoctorBSTNode* node, int id) const {
        if (!node || node->data.id == id) {
            return node;
        }
        if (id < node->data.id) {
            return searchRecursive(node->left, id);
        }
        return searchRecursive(node->right, id);
    }

    void inOrderRecursive(DoctorBSTNode* node, std::vector<Doctor>& result) const {
        if (!node) return;
        inOrderRecursive(node->left, result);
        result.push_back(node->data);
        inOrderRecursive(node->right, result);
    }

    void reverseInOrderRecursive(DoctorBSTNode* node, std::vector<Doctor>& result) const {
        if (!node) return;
        reverseInOrderRecursive(node->right, result);
        result.push_back(node->data);
        reverseInOrderRecursive(node->left, result);
    }

    void searchSpecialtyRecursive(DoctorBSTNode* node, const std::string& specialty,
                                 std::vector<Doctor>& result) const {
        if (!node) return;
        searchSpecialtyRecursive(node->left, specialty, result);
        std::string nodeSpec = toLower(node->data.specialty);
        if (nodeSpec.find(specialty) != std::string::npos) {
            result.push_back(node->data);
        }
        searchSpecialtyRecursive(node->right, specialty, result);
    }

    void rangeQueryRecursive(DoctorBSTNode* node, int minId, int maxId,
                            std::vector<Doctor>& result) const {
        if (!node) return;
        if (node->data.id > minId) {
            rangeQueryRecursive(node->left, minId, maxId, result);
        }
        if (node->data.id >= minId && node->data.id <= maxId) {
            result.push_back(node->data);
        }
        if (node->data.id < maxId) {
            rangeQueryRecursive(node->right, minId, maxId, result);
        }
    }

    DoctorBSTNode* findMin(DoctorBSTNode* node) const {
        while (node && node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    DoctorBSTNode* removeRecursive(DoctorBSTNode* node, int id) {
        if (!node) return nullptr;

        if (id < node->data.id) {
            node->left = removeRecursive(node->left, id);
        } else if (id > node->data.id) {
            node->right = removeRecursive(node->right, id);
        } else {
            // Node found
            if (!node->left) {
                DoctorBSTNode* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                DoctorBSTNode* temp = node->left;
                delete node;
                return temp;
            }

            // Node with two children: get inorder successor (smallest in right subtree)
            DoctorBSTNode* temp = findMin(node->right);
            node->data = temp->data;
            node->right = removeRecursive(node->right, temp->data.id);
        }
        return node;
    }

public:
    DoctorBST() = default;

    ~DoctorBST() {
        clear();
    }

    DoctorBST(const DoctorBST&) = delete;
    DoctorBST& operator=(const DoctorBST&) = delete;

    void clear() {
        destroyRecursive(root);
        root = nullptr;
    }

    void buildFromVector(const std::vector<Doctor>& doctors) {
        clear();
        for (const auto& doc : doctors) {
            insert(doc);
        }
    }

    void insert(const Doctor& doc) {
        root = insertRecursive(root, doc);
    }

    bool remove(int id) {
        if (!search(id)) return false;
        root = removeRecursive(root, id);
        return true;
    }

    Doctor* search(int id) const {
        DoctorBSTNode* found = searchRecursive(root, id);
        return found ? &(found->data) : nullptr;
    }

    // In-order traversal yields doctors sorted by ID ascending
    std::vector<Doctor> getSortedDoctorsAscending() const {
        std::vector<Doctor> result;
        inOrderRecursive(root, result);
        return result;
    }

    // Reverse in-order traversal yields doctors sorted by ID descending
    std::vector<Doctor> getSortedDoctorsDescending() const {
        std::vector<Doctor> result;
        reverseInOrderRecursive(root, result);
        return result;
    }

    // Recursive search for doctors matching specialty
    std::vector<Doctor> searchBySpecialty(const std::string& specialty) const {
        std::vector<Doctor> result;
        searchSpecialtyRecursive(root, toLower(specialty), result);
        return result;
    }

    // Range query between minId and maxId
    std::vector<Doctor> getDoctorsInRange(int minId, int maxId) const {
        std::vector<Doctor> result;
        rangeQueryRecursive(root, minId, maxId, result);
        return result;
    }
};

#endif
