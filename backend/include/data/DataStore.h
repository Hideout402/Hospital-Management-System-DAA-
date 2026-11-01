#ifndef DATASTORE_H
#define DATASTORE_H

#include "MockData.h"

class DataStore {
public:
    static vector<Patient>& getPatients() {
        return MockData::patients;
    }

    static vector<Doctor>& getDoctors() {
        return MockData::doctors;
    }

    static vector<Department>& getDepartments() {
        return MockData::departments;
    }

    static vector<Appointment>& getAppointments() {
        return MockData::appointments;
    }

    static vector<Bill>& getBills() {
        return MockData::bills;
    }

    static vector<EmergencyCase>& getEmergencyCases() {
        return MockData::emergencyCases;
    }

    static vector<HospitalLocation>& getHospitalLocations() {
        return MockData::hospitalLocations;
    }
};

#endif
