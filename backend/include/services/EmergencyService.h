#ifndef EMERGENCYSERVICE_H
#define EMERGENCYSERVICE_H

#include "../data/DataStore.h"
#include "../dsa/PriorityQueue.h"

class EmergencyService
{
public:
    static vector<EmergencyCase>& getAllCases()
    {
        return DataStore::getEmergencyCases();
    }

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

    static vector<EmergencyCase> getPriorityOrder()
    {
        vector<EmergencyCase> result = DataStore::getEmergencyCases();

        // Sort by triage level, then arrival order.
        for (int i = 0; i < result.size(); i++)
        {
            for (int j = 0; j < result.size() - i - 1; j++)
            {
                if (result[j].triageLevel > result[j + 1].triageLevel ||
                    (result[j].triageLevel == result[j + 1].triageLevel &&
                     result[j].arrivalOrder > result[j + 1].arrivalOrder))
                {
                    swap(result[j], result[j + 1]);
                }
            }
        }

        return result;
    }

    static bool completeCase(int id)
    {
        for (auto& c : DataStore::getEmergencyCases())
        {
            if (c.id == id)
            {
                c.status = "Completed";
                return true;
            }
        }

        return false;
    }
};

#endif
