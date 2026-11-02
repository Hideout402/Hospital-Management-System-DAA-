#ifndef DEPARTMENTTREE_H
#define DEPARTMENTTREE_H

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>
#include "../models/Department.h"
#include "../libraries/json.hpp"

struct DepartmentTreeNode {
    Department data;
    DepartmentTreeNode* parent = nullptr;
    std::vector<DepartmentTreeNode*> children;
    int depth = 0;

    DepartmentTreeNode() = default;
    explicit DepartmentTreeNode(const Department& dept)
        : data(dept), parent(nullptr), depth(0) {}
};

class DepartmentTree {
private:
    DepartmentTreeNode* virtualRoot;

    static std::string toLower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return str;
    }

    void destroyRecursive(DepartmentTreeNode* node) {
        if (!node) return;
        for (auto* child : node->children) {
            destroyRecursive(child);
        }
        delete node;
    }

    DepartmentTreeNode* findRecursive(DepartmentTreeNode* current, int id) const {
        if (!current) return nullptr;
        if (current != virtualRoot && current->data.id == id) {
            return current;
        }
        for (auto* child : current->children) {
            DepartmentTreeNode* found = findRecursive(child, id);
            if (found) return found;
        }
        return nullptr;
    }

    void searchRecursive(DepartmentTreeNode* current, const std::string& query,
                         std::vector<Department>& results) const {
        if (!current) return;
        if (current != virtualRoot) {
            std::string nameLower = toLower(current->data.name);
            std::string descLower = toLower(current->data.description);
            std::string locLower = toLower(current->data.location);
            if (nameLower.find(query) != std::string::npos ||
                descLower.find(query) != std::string::npos ||
                locLower.find(query) != std::string::npos) {
                results.push_back(current->data);
            }
        }
        for (auto* child : current->children) {
            searchRecursive(child, query, results);
        }
    }

    void collectSubtreeIdsRecursive(DepartmentTreeNode* node, std::vector<int>& ids) const {
        if (!node) return;
        if (node != virtualRoot) {
            ids.push_back(node->data.id);
        }
        for (auto* child : node->children) {
            collectSubtreeIdsRecursive(child, ids);
        }
    }

    void collectSubdepartmentsRecursive(DepartmentTreeNode* node, std::vector<Department>& result) const {
        if (!node) return;
        for (auto* child : node->children) {
            result.push_back(child->data);
            collectSubdepartmentsRecursive(child, result);
        }
    }

    int computeHeightRecursive(DepartmentTreeNode* node) const {
        if (!node) return 0;
        int maxHeight = 0;
        for (auto* child : node->children) {
            maxHeight = std::max(maxHeight, computeHeightRecursive(child));
        }
        return (node == virtualRoot) ? maxHeight : (1 + maxHeight);
    }

    int countNodesRecursive(DepartmentTreeNode* node) const {
        if (!node) return 0;
        int count = (node == virtualRoot) ? 0 : 1;
        for (auto* child : node->children) {
            count += countNodesRecursive(child);
        }
        return count;
    }

    nlohmann::json nodeToJsonRecursive(DepartmentTreeNode* node) const {
        if (!node) return nlohmann::json::object();

        nlohmann::json j;
        j["id"] = node->data.id;
        j["name"] = node->data.name;
        j["parentDepartmentId"] = node->data.parentDepartmentId;
        j["description"] = node->data.description;
        j["location"] = node->data.location;
        j["headDoctor"] = node->data.headDoctor;
        j["status"] = node->data.status;
        j["depth"] = node->depth;

        nlohmann::json childrenArray = nlohmann::json::array();
        for (auto* child : node->children) {
            childrenArray.push_back(nodeToJsonRecursive(child));
        }
        j["children"] = childrenArray;
        j["childCount"] = static_cast<int>(node->children.size());

        return j;
    }

public:
    DepartmentTree() {
        Department rootDept(0, "CarePoint Hospital System", 0,
                           "Hospital Root Directorate", "Main Campus",
                           "Chief Medical Officer", "Active");
        virtualRoot = new DepartmentTreeNode(rootDept);
    }

    ~DepartmentTree() {
        clear();
        delete virtualRoot;
        virtualRoot = nullptr;
    }

    DepartmentTree(const DepartmentTree&) = delete;
    DepartmentTree& operator=(const DepartmentTree&) = delete;

    void clear() {
        if (virtualRoot) {
            for (auto* child : virtualRoot->children) {
                destroyRecursive(child);
            }
            virtualRoot->children.clear();
        }
    }

    // Build the N-ary tree from a flat list of departments
    void buildFromList(const std::vector<Department>& departments) {
        clear();

        std::unordered_map<int, DepartmentTreeNode*> nodeMap;

        // Step 1: Create tree nodes
        for (const auto& dept : departments) {
            nodeMap[dept.id] = new DepartmentTreeNode(dept);
        }

        // Step 2: Establish parent-child links
        for (const auto& dept : departments) {
            DepartmentTreeNode* current = nodeMap[dept.id];
            if (dept.parentDepartmentId > 0 && nodeMap.find(dept.parentDepartmentId) != nodeMap.end()) {
                DepartmentTreeNode* parentNode = nodeMap[dept.parentDepartmentId];
                current->parent = parentNode;
                parentNode->children.push_back(current);
            } else {
                current->parent = virtualRoot;
                virtualRoot->children.push_back(current);
            }
        }

        // Step 3: Compute depth levels recursively
        std::vector<std::pair<DepartmentTreeNode*, int>> queue;
        for (auto* child : virtualRoot->children) {
            queue.push_back({child, 1});
        }
        size_t headIdx = 0;
        while (headIdx < queue.size()) {
            auto [node, d] = queue[headIdx++];
            node->depth = d;
            for (auto* child : node->children) {
                queue.push_back({child, d + 1});
            }
        }
    }

    // Find node by department ID
    DepartmentTreeNode* findNode(int id) const {
        return findRecursive(virtualRoot, id);
    }

    // Recursive search across department tree
    std::vector<Department> searchByName(const std::string& query) const {
        std::vector<Department> results;
        if (query.empty()) return results;
        searchRecursive(virtualRoot, toLower(query), results);
        return results;
    }

    // Collect department ID and all descendant sub-unit IDs (recursive)
    std::vector<int> getSubtreeDepartmentIds(int departmentId) const {
        std::vector<int> ids;
        DepartmentTreeNode* node = findNode(departmentId);
        if (node) {
            collectSubtreeIdsRecursive(node, ids);
        }
        return ids;
    }

    // Get subdepartments of a given department (either immediate or all recursive)
    std::vector<Department> getSubdepartments(int departmentId, bool recursive = true) const {
        std::vector<Department> results;
        DepartmentTreeNode* node = findNode(departmentId);
        if (!node) return results;

        if (recursive) {
            collectSubdepartmentsRecursive(node, results);
        } else {
            for (auto* child : node->children) {
                results.push_back(child->data);
            }
        }
        return results;
    }

    // Get breadcrumb path from top root to this department
    std::vector<std::string> getBreadcrumbPath(int departmentId) const {
        std::vector<std::string> path;
        DepartmentTreeNode* current = findNode(departmentId);
        while (current && current != virtualRoot) {
            path.push_back(current->data.name);
            current = current->parent;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    // Prevent cycles: check if targetId is an ancestor of potentialChildId
    bool isDescendant(int ancestorId, int targetId) const {
        if (ancestorId == targetId) return true;
        DepartmentTreeNode* current = findNode(targetId);
        while (current && current != virtualRoot) {
            if (current->data.id == ancestorId) return true;
            current = current->parent;
        }
        return false;
    }

    int getHeight() const {
        return computeHeightRecursive(virtualRoot);
    }

    int getTotalDepartments() const {
        return countNodesRecursive(virtualRoot);
    }

    // Return the full hierarchy as a nested JSON structure
    nlohmann::json getHierarchyJson() const {
        nlohmann::json result = nlohmann::json::array();
        if (!virtualRoot) return result;

        for (auto* child : virtualRoot->children) {
            result.push_back(nodeToJsonRecursive(child));
        }
        return result;
    }
};

#endif
