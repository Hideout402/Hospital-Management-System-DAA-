#ifndef EMERGENCYSERVICE_H
#define EMERGENCYSERVICE_H

#include <string>
#include "../data/DataStore.h"
#include "../dsa/PriorityQueue.h"

class EmergencyService
{
public:
    static vector<EmergencyCase>& getAllCases()
    {
        return DataStore::getEmergencyCases();
    }

    static EmergencyCase* findCase(int id)
    {
        for (auto& c : DataStore::getEmergencyCases())
        {
            if (c.id == id)
                return &c;
        }
        return nullptr;
    }

    static bool patientExists(int patientId)
    {
        for (const auto& p : DataStore::getPatients())
        {
            if (p.id == patientId)
                return true;
        }
        return false;
    }

    static string getPatientName(int patientId)
    {
        for (const auto& p : DataStore::getPatients())
        {
            if (p.id == patientId)
                return p.name;
        }
        return "Unknown patient";
    }

    // Original function kept: add a case that already has an id
    static bool addEmergencyCase(EmergencyCase emergencyCase)
    {
        auto& cases = DataStore::getEmergencyCases();

        for (const auto& c : cases)
        {
            if (c.id == emergencyCase.id)
                return false;
        }

        cases.push_back(emergencyCase);
        return true;
    }

    // New case from the API: id and arrivalOrder are generated automatically
    static void createCase(int patientId, int triageLevel, const string& notes, EmergencyCase& created)
    {
        auto& cases = DataStore::getEmergencyCases();

        int maxId = 0;
        int maxOrder = 0;
        for (const auto& c : cases)
        {
            if (c.id > maxId) maxId = c.id;
            if (c.arrivalOrder > maxOrder) maxOrder = c.arrivalOrder;
        }

        created = EmergencyCase{maxId + 1, patientId, triageLevel, maxOrder + 1, "Waiting", notes};
        cases.push_back(created);
    }

    // Uses the manual min-heap. Unfinished cases come out most urgent first,
    // then the completed cases are added at the end.
    static vector<EmergencyCase> getPriorityOrder()
    {
        EmergencyPriorityQueue pq;
        vector<EmergencyCase> finished;

        for (const auto& c : DataStore::getEmergencyCases())
        {
            if (c.status == "Completed")
                finished.push_back(c);
            else
                pq.insert(c);
        }

        vector<EmergencyCase> result;
        EmergencyCase next;
        while (pq.extractMin(next))
            result.push_back(next);

        for (const auto& c : finished)
            result.push_back(c);

        return result;
    }

    // Takes the most urgent WAITING case out of the heap and marks it In Treatment
    static bool treatNext(EmergencyCase& treated)
    {
        EmergencyPriorityQueue pq;

        for (const auto& c : DataStore::getEmergencyCases())
        {
            if (c.status == "Waiting")
                pq.insert(c);
        }

        EmergencyCase next;
        if (!pq.extractMin(next))
            return false;

        EmergencyCase* stored = findCase(next.id);
        if (stored == nullptr)
            return false;

        stored->status = "In Treatment";
        treated = *stored;
        return true;
    }

    static bool isValidStatus(const string& status)
    {
        return status == "Waiting" || status == "In Treatment" || status == "Completed";
    }

    static bool updateStatus(int id, const string& status)
    {
        EmergencyCase* c = findCase(id);
        if (c == nullptr)
            return false;

        c->status = status;
        return true;
    }

    // Original function kept
    static bool completeCase(int id)
    {
        return updateStatus(id, "Completed");
    }
};

#endif