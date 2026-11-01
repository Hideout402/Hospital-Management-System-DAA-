#ifndef PATIENTSERVICE_H
#define PATIENTSERVICE_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "../data/DataStore.h"
#include "../dsa/Searching.h"
#include "../dsa/HashTable.h"
#include "../dsa/LinkedLists.h"
#include "../dsa/Sorting.h"

using namespace std;

class PatientService
{
public:

    // Get all patients
    static vector<Patient>& getAllPatients()
    {
        return DataStore::getPatients();
    }

    // Hash Table: Find patient by ID
    static Patient* findPatient(int id)
    {
        vector<Patient>& patients = DataStore::getPatients();

        SimpleHashTable patientTable;

        for (const Patient& patient : patients)
        {
            patientTable.insert(patient.id);
        }

        if (patientTable.search(id))
        {
            for (Patient& patient : patients)
            {
                if (patient.id == id)
                    return &patient;
            }
        }

        return nullptr;
    }

    // Add patient
    static bool addPatient(Patient patient)
    {
        vector<Patient>& patients = DataStore::getPatients();

        if (findPatient(patient.id) != nullptr)
            return false;

        patients.push_back(patient);
        return true;
    }

    // Delete patient
    static bool deletePatient(int id)
    {
        vector<Patient>& patients = DataStore::getPatients();

        for (int i = 0; i < patients.size(); i++)
        {
            if (patients[i].id == id)
            {
                patients.erase(patients.begin() + i);
                return true;
            }
        }

        return false;
    }

    // Update patient
    static bool updatePatient(Patient updatedPatient)
    {
        Patient* patient = findPatient(updatedPatient.id);

        if (patient == nullptr)
            return false;

        *patient = updatedPatient;
        return true;
    }

    // Linear Search: Search patients by name
    static vector<Patient> searchPatient(string name)
    {
        vector<Patient> results;
        vector<Patient>& patients = DataStore::getPatients();

        vector<string> patientNames;

        for (const Patient& patient : patients)
        {
            patientNames.push_back(patient.name);
        }

        int index = linearSearch(patientNames, name);

        if (index != -1)
        {
            results.push_back(patients[index]);
        }

        return results;
    }

    // Linked List: Return patient visit history using the existing singly linked list traversal
    static bool getVisitHistoryTraversal(int id, vector<string>& result)
    {
        Patient* patient = findPatient(id);

        if (patient == nullptr)
            return false;

        SinglyLinkedList history;

        for (const string& visit : patient->visitHistory)
        {
            history.insert(visit);
        }

        result = history.toVector();
        return true;
    }

    static bool displayVisitHistory(int id)
    {
        Patient* patient = findPatient(id);

        if (patient == nullptr)
            return false;

        vector<string> visits;

        if (!getVisitHistoryTraversal(id, visits))
            return false;

        cout << "Visit History of " << patient->name << ":" << endl;

        for (const string& visit : visits)
        {
            cout << visit << " -> ";
        }

        cout << "NULL" << endl;

        return true;
    }

    // Bubble Sort: Sort patients by age
    static vector<Patient> sortPatientsByAge()
    {
        vector<Patient> patients = DataStore::getPatients();

        bubbleSort(patients,
            [](const Patient& a, const Patient& b)
            {
                return a.age < b.age;
            });

        return patients;
    }

    // Merge Sort: Sort patients by name
    static vector<Patient> sortPatientsByName()
    {
        vector<Patient> patients = DataStore::getPatients();

        if (!patients.empty())
        {
            mergeSort(patients, 0, patients.size() - 1,
                [](const Patient& a, const Patient& b)
                {
                    return a.name < b.name;
                });
        }

        return patients;
    }

    // Binary Search: Find patient by ID
    static Patient* findPatientBinary(int id)
    {
        vector<Patient>& patients = DataStore::getPatients();

        vector<int> patientIDs;

        for (const Patient& patient : patients)
        {
            patientIDs.push_back(patient.id);
        }

        // Sort IDs before applying Binary Search
        bubbleSort(patientIDs);

        int index = binarySearch(patientIDs, id);

        if (index == -1)
            return nullptr;

        // Find the original patient record
        for (Patient& patient : patients)
        {
            if (patient.id == patientIDs[index])
                return &patient;
        }

        return nullptr;
    }
};

#endif