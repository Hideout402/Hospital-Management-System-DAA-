#ifndef DOCTORSERVICE_H
#define DOCTORSERVICE_H

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <vector>
#include "../data/DataStore.h"
#include "../dsa/DoctorBST.h"
#include "../dsa/DoctorList.h"
#include "../dsa/Sorting.h"
#include "DepartmentService.h"

class DoctorService
{
private:
    static inline std::unique_ptr<DoctorDoublyLinkedList> rosterInstance = nullptr;
    static inline std::unique_ptr<DoctorBST> bstInstance = nullptr;

    static std::string toLower(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return str;
    }

    static void ensureDsaInitialized()
    {
        if (!rosterInstance)
        {
            rosterInstance = std::make_unique<DoctorDoublyLinkedList>();
            rosterInstance->buildFromVector(DataStore::getDoctors());
        }
        if (!bstInstance)
        {
            bstInstance = std::make_unique<DoctorBST>();
            bstInstance->buildFromVector(DataStore::getDoctors());
        }
    }

public:
    static void refreshDsa()
    {
        ensureDsaInitialized();
        rosterInstance->buildFromVector(DataStore::getDoctors());
        bstInstance->buildFromVector(DataStore::getDoctors());
    }

    static DoctorDoublyLinkedList& getRosterList()
    {
        ensureDsaInitialized();
        return *rosterInstance;
    }

    static DoctorBST& getBST()
    {
        ensureDsaInitialized();
        return *bstInstance;
    }

    static std::vector<Doctor>& getAllDoctors()
    {
        return DataStore::getDoctors();
    }

    static Doctor* findDoctor(int id)
    {
        for (auto& doctor : DataStore::getDoctors())
        {
            if (doctor.id == id)
                return &doctor;
        }
        return nullptr;
    }

    static bool addDoctor(Doctor& doctor)
    {
        auto& doctors = DataStore::getDoctors();

        // Auto-generate ID if <= 0
        if (doctor.id <= 0)
        {
            int maxId = 0;
            for (const auto& d : doctors)
            {
                if (d.id > maxId) maxId = d.id;
            }
            doctor.id = maxId + 1;
        }
        else if (findDoctor(doctor.id) != nullptr)
        {
            return false; // Duplicate doctor ID
        }

        // Validate department exists
        if (doctor.departmentId > 0 &&
            DepartmentService::findDepartment(doctor.departmentId) == nullptr)
        {
            return false; // Department doesn't exist
        }

        doctors.push_back(doctor);
        refreshDsa();
        return true;
    }

    static bool updateDoctor(int id, const Doctor& updated, std::string& errorMsg)
    {
        Doctor* existing = findDoctor(id);
        if (!existing)
        {
            errorMsg = "Doctor not found.";
            return false;
        }

        // Validate department if changing
        if (updated.departmentId > 0 &&
            DepartmentService::findDepartment(updated.departmentId) == nullptr)
        {
            errorMsg = "Assigned department does not exist.";
            return false;
        }

        existing->name = updated.name;
        existing->specialty = updated.specialty;
        if (updated.departmentId > 0) existing->departmentId = updated.departmentId;
        if (!updated.contact.empty()) existing->contact = updated.contact;
        if (!updated.email.empty()) existing->email = updated.email;
        if (!updated.roomNumber.empty()) existing->roomNumber = updated.roomNumber;
        if (!updated.qualification.empty()) existing->qualification = updated.qualification;
        if (updated.experienceYears >= 0) existing->experienceYears = updated.experienceYears;
        if (!updated.availability.empty()) existing->availability = updated.availability;
        if (!updated.availableSlots.empty()) existing->availableSlots = updated.availableSlots;
        if (updated.consultationFee >= 0) existing->consultationFee = updated.consultationFee;

        refreshDsa();
        return true;
    }

    static bool deleteDoctor(int id)
    {
        auto& doctors = DataStore::getDoctors();

        for (size_t i = 0; i < doctors.size(); i++)
        {
            if (doctors[i].id == id)
            {
                doctors.erase(doctors.begin() + i);
                refreshDsa();
                return true;
            }
        }
        return false;
    }

    // Filter doctors with optional hierarchy subdepartment resolution
    static std::vector<Doctor> filterDoctors(int departmentId = 0,
                                            const std::string& specialty = "",
                                            const std::string& availability = "",
                                            const std::string& search = "",
                                            bool includeSubdepartments = false)
    {
        const auto& allDoctors = DataStore::getDoctors();
        std::vector<Doctor> filtered;

        std::vector<int> targetDeptIds;
        if (departmentId > 0)
        {
            if (includeSubdepartments)
            {
                targetDeptIds = DepartmentService::getSubtreeDepartmentIds(departmentId);
            }
            else
            {
                targetDeptIds.push_back(departmentId);
            }
        }

        const std::string searchLower = toLower(search);
        const std::string specLower = toLower(specialty);

        for (const auto& doc : allDoctors)
        {
            // Department check
            if (!targetDeptIds.empty())
            {
                if (std::find(targetDeptIds.begin(), targetDeptIds.end(), doc.departmentId) == targetDeptIds.end())
                {
                    continue;
                }
            }

            // Specialty check
            if (!specLower.empty())
            {
                std::string docSpec = toLower(doc.specialty);
                if (docSpec.find(specLower) == std::string::npos &&
                    specLower.find(docSpec) == std::string::npos)
                {
                    continue;
                }
            }

            // Availability check
            if (!availability.empty() && availability != "All")
            {
                if (doc.availability != availability)
                {
                    continue;
                }
            }

            // Search query check (name, room, qualification)
            if (!searchLower.empty())
            {
                std::string nameLower = toLower(doc.name);
                std::string roomLower = toLower(doc.roomNumber);
                std::string qualLower = toLower(doc.qualification);
                std::string docSpec = toLower(doc.specialty);

                if (nameLower.find(searchLower) == std::string::npos &&
                    roomLower.find(searchLower) == std::string::npos &&
                    qualLower.find(searchLower) == std::string::npos &&
                    docSpec.find(searchLower) == std::string::npos)
                {
                    continue;
                }
            }

            filtered.push_back(doc);
        }

        return filtered;
    }

    // Multi-criteria sorting using Merge Sort and BST
    static std::vector<Doctor> sortDoctors(std::vector<Doctor> docs,
                                           const std::string& sortBy,
                                           const std::string& sortOrder)
    {
        if (docs.empty()) return docs;

        const bool ascending = (sortOrder != "desc" && sortOrder != "DESC");

        // When sorting by ID and no filters applied, can utilize BST in-order traversal
        if (sortBy == "id" && docs.size() == DataStore::getDoctors().size())
        {
            return ascending ? getBST().getSortedDoctorsAscending()
                             : getBST().getSortedDoctorsDescending();
        }

        if (sortBy == "name")
        {
            mergeSort(docs, 0, static_cast<int>(docs.size()) - 1,
                      [ascending](const Doctor& a, const Doctor& b) {
                          return ascending ? (a.name < b.name) : (a.name > b.name);
                      });
        }
        else if (sortBy == "specialty" || sortBy == "specialization")
        {
            mergeSort(docs, 0, static_cast<int>(docs.size()) - 1,
                      [ascending](const Doctor& a, const Doctor& b) {
                          return ascending ? (a.specialty < b.specialty) : (a.specialty > b.specialty);
                      });
        }
        else if (sortBy == "fee" || sortBy == "consultationFee")
        {
            mergeSort(docs, 0, static_cast<int>(docs.size()) - 1,
                      [ascending](const Doctor& a, const Doctor& b) {
                          return ascending ? (a.consultationFee < b.consultationFee)
                                           : (a.consultationFee > b.consultationFee);
                      });
        }
        else if (sortBy == "experience" || sortBy == "experienceYears")
        {
            mergeSort(docs, 0, static_cast<int>(docs.size()) - 1,
                      [ascending](const Doctor& a, const Doctor& b) {
                          return ascending ? (a.experienceYears < b.experienceYears)
                                           : (a.experienceYears > b.experienceYears);
                      });
        }
        else // default to ID
        {
            mergeSort(docs, 0, static_cast<int>(docs.size()) - 1,
                      [ascending](const Doctor& a, const Doctor& b) {
                          return ascending ? (a.id < b.id) : (a.id > b.id);
                      });
        }

        return docs;
    }

    // Doubly Linked List active roster operations (Bi-directional traversal)
    static std::vector<Doctor> getRoster(bool forward = true,
                                         const std::string& availabilityFilter = "")
    {
        ensureDsaInitialized();
        if (availabilityFilter.empty() || availabilityFilter == "All")
        {
            return forward ? getRosterList().toVectorForward()
                           : getRosterList().toVectorBackward();
        }
        return getRosterList().filterByAvailability(availabilityFilter, forward);
    }

    // Shift rotation: get next doctor in active roster
    static const Doctor* getNextOnCallDoctor(int currentDoctorId)
    {
        ensureDsaInitialized();
        return getRosterList().getNextDoctor(currentDoctorId, true);
    }

    // Shift rotation: get previous doctor in active roster
    static const Doctor* getPreviousOnCallDoctor(int currentDoctorId)
    {
        ensureDsaInitialized();
        return getRosterList().getPrevDoctor(currentDoctorId, true);
    }

    // Binary Search Tree: In-order traversal
    static std::vector<Doctor> getBstSortedDoctors()
    {
        ensureDsaInitialized();
        return getBST().getSortedDoctorsAscending();
    }

    // Binary Search Tree: Recursive range query
    static std::vector<Doctor> getDoctorsInRange(int minId, int maxId)
    {
        ensureDsaInitialized();
        return getBST().getDoctorsInRange(minId, maxId);
    }

    // Backward-compatibility department delegates
    static std::vector<Department>& getDepartments()
    {
        return DepartmentService::getAllDepartments();
    }

    static bool addDepartment(Department department)
    {
        return DepartmentService::addDepartment(department);
    }
};

#endif
