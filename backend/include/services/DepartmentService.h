#ifndef DEPARTMENTSERVICE_H
#define DEPARTMENTSERVICE_H

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "../data/DataStore.h"
#include "../dsa/DepartmentTree.h"
#include "../libraries/json.hpp"

class DepartmentService {
private:
    static inline std::unique_ptr<DepartmentTree> treeInstance = nullptr;

    static void ensureTreeInitialized() {
        if (!treeInstance) {
            treeInstance = std::make_unique<DepartmentTree>();
            treeInstance->buildFromList(DataStore::getDepartments());
        }
    }

public:
    static void refreshTree() {
        ensureTreeInitialized();
        treeInstance->buildFromList(DataStore::getDepartments());
    }

    static DepartmentTree& getTree() {
        ensureTreeInitialized();
        return *treeInstance;
    }

    static std::vector<Department>& getAllDepartments() {
        return DataStore::getDepartments();
    }

    static Department* findDepartment(int id) {
        for (auto& dept : DataStore::getDepartments()) {
            if (dept.id == id) {
                return &dept;
            }
        }
        return nullptr;
    }

    static bool addDepartment(Department& department) {
        auto& depts = DataStore::getDepartments();

        // Auto-generate ID if <= 0
        if (department.id <= 0) {
            int maxId = 0;
            for (const auto& d : depts) {
                if (d.id > maxId) maxId = d.id;
            }
            department.id = maxId + 1;
        } else if (findDepartment(department.id) != nullptr) {
            return false; // Duplicate ID
        }

        // Validate parent if specified
        if (department.parentDepartmentId > 0 &&
            findDepartment(department.parentDepartmentId) == nullptr) {
            return false; // Parent department does not exist
        }

        depts.push_back(department);
        refreshTree();
        return true;
    }

    static bool updateDepartment(int id, const Department& updated, std::string& errorMsg) {
        Department* existing = findDepartment(id);
        if (!existing) {
            errorMsg = "Department not found.";
            return false;
        }

        if (updated.parentDepartmentId == id) {
            errorMsg = "A department cannot be its own parent.";
            return false;
        }

        // Validate parent exists if > 0
        if (updated.parentDepartmentId > 0) {
            Department* parentDept = findDepartment(updated.parentDepartmentId);
            if (!parentDept) {
                errorMsg = "Parent department does not exist.";
                return false;
            }

            // Cycle check using the General Tree
            if (getTree().isDescendant(id, updated.parentDepartmentId)) {
                errorMsg = "Invalid hierarchy: setting parent would create a cycle in the department tree.";
                return false;
            }
        }

        existing->name = updated.name;
        existing->parentDepartmentId = updated.parentDepartmentId;
        if (!updated.description.empty()) existing->description = updated.description;
        if (!updated.location.empty()) existing->location = updated.location;
        if (!updated.headDoctor.empty()) existing->headDoctor = updated.headDoctor;
        if (!updated.status.empty()) existing->status = updated.status;

        refreshTree();
        return true;
    }

    static bool deleteDepartment(int id, std::string& errorMsg) {
        auto& depts = DataStore::getDepartments();

        Department* dept = findDepartment(id);
        if (!dept) {
            errorMsg = "Department not found.";
            return false;
        }

        // Check if any doctors are currently assigned to this department
        for (const auto& doc : DataStore::getDoctors()) {
            if (doc.departmentId == id) {
                errorMsg = "Cannot delete department: Doctors are currently assigned to this department.";
                return false;
            }
        }

        const int oldParentId = dept->parentDepartmentId;

        // Reparent direct child departments to the parent of this department
        for (auto& d : depts) {
            if (d.parentDepartmentId == id) {
                d.parentDepartmentId = oldParentId;
            }
        }

        // Remove the department
        for (auto it = depts.begin(); it != depts.end(); ++it) {
            if (it->id == id) {
                depts.erase(it);
                break;
            }
        }

        refreshTree();
        return true;
    }

    // Get the N-ary tree nested JSON representation
    static nlohmann::json getHierarchyJson() {
        return getTree().getHierarchyJson();
    }

    // Recursive traversal to collect all department IDs in subtree
    static std::vector<int> getSubtreeDepartmentIds(int id) {
        return getTree().getSubtreeDepartmentIds(id);
    }

    // Recursive search for subdepartments
    static std::vector<Department> getSubdepartments(int id, bool recursive = true) {
        return getTree().getSubdepartments(id, recursive);
    }

    // Get breadcrumb path from root to node
    static std::vector<std::string> getDepartmentPath(int id) {
        return getTree().getBreadcrumbPath(id);
    }

    // Recursive search across department tree
    static std::vector<Department> searchDepartments(const std::string& query) {
        return getTree().searchByName(query);
    }

    // Department operational statistics
    static nlohmann::json getDepartmentStats() {
        ensureTreeInitialized();
        const auto& depts = DataStore::getDepartments();
        const auto& docs = DataStore::getDoctors();

        nlohmann::json doctorCountPerDept = nlohmann::json::object();
        for (const auto& d : depts) {
            doctorCountPerDept[std::to_string(d.id)] = 0;
        }
        for (const auto& doc : docs) {
            std::string key = std::to_string(doc.departmentId);
            if (doctorCountPerDept.contains(key)) {
                doctorCountPerDept[key] = doctorCountPerDept[key].get<int>() + 1;
            }
        }

        return {
            {"totalDepartments", depts.size()},
            {"treeHeight", getTree().getHeight()},
            {"doctorCounts", doctorCountPerDept}
        };
    }
};

#endif
