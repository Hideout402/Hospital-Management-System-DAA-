#ifndef MOCKDATA_H
#define MOCKDATA_H

#include <vector>
#include "../models/Patient.h"
#include "../models/Doctor.h"
#include "../models/Department.h"
#include "../models/Appointment.h"
#include "../models/Bill.h"
#include "../models/EmergencyCase.h"
#include "../models/HospitalLocation.h"

using namespace std;

namespace MockData {

    
inline vector<Patient> patients = {
    {1001, "Ayesha Khan", 28, "Female", "0301-1234567",
 "Karachi", "Cardiology", "Active", {"General Checkup", "ECG Test", "Follow-up Consultation"}},

    {1002, "Hamza Malik", 42, "Male", "0302-2345678",
 "Gulshan", "Neurology", "Active", {"Blood Test", "MRI Review"}},

    {1003, "Fatima Iqbal", 35, "Female", "0303-3456789",
 "Johar", "Orthopedics", "Discharged", {"X-Ray", "Physiotherapy Session", "Discharge Review"}},

    {1004, "Ali Raza", 51, "Male", "0304-4567890",
 "Karachi", "General Medicine", "Active", {"Initial Assessment"}},

    {1005, "Hira Ahmed", 23, "Female", "0305-5678901",
 "Karachi", "Dermatology", "Discharged", {"Skin Consultation"}}
};

    inline vector<Doctor> doctors = {
        {1, "Dr. Ahmed", "Cardiologist", 1, {"09:00", "10:00"}, 1500},
        {2, "Dr. Sara", "Neurologist", 2, {"11:00", "12:00"}, 2000},
        {3, "Dr. Usman", "Orthopedic", 3, {"13:00", "14:00"}, 1800}
    };

    inline vector<Department> departments = {
        {1, "Cardiology", 0},
        {2, "Neurology", 0},
        {3, "Orthopedics", 0}
    };

    inline vector<Appointment> appointments = {
        {1, 1, 1, "2026-11-01", "09:00", "Scheduled"},
        {2, 2, 2, "2026-11-01", "11:00", "Scheduled"}
    };

    inline vector<Bill> bills = {
        {1, 1001, "Ayesha Khan",
         {{"Consultation", 1, 2500.0, 2500.0}, {"Diagnostic Lab", 1, 1200.0, 1200.0}},
         3700.0, 300.0, 3400.0, 2500.0, 900.0, "Partially Paid"},
        {2, 1002, "Hamza Malik",
         {{"Neurology Consultation", 1, 3200.0, 3200.0}, {"MRI Scan", 1, 5600.0, 5600.0}},
         8800.0, 500.0, 8300.0, 8300.0, 0.0, "Paid"},
        {3, 1003, "Fatima Iqbal",
         {{"Orthopedic Review", 1, 2100.0, 2100.0}, {"Physiotherapy", 2, 1800.0, 3600.0}},
         5700.0, 0.0, 5700.0, 1500.0, 4200.0, "Unpaid"},
        {4, 1004, "Ali Raza",
         {{"General Medicine", 1, 1800.0, 1800.0}},
         1800.0, 0.0, 1800.0, 0.0, 1800.0, "Unpaid"},
        {5, 1005, "Hira Ahmed",
         {{"Dermatology Consultation", 1, 2200.0, 2200.0}, {"Medication", 1, 850.0, 850.0}},
         3050.0, 150.0, 2900.0, 2900.0, 0.0, "Paid"}
    };

    inline vector<EmergencyCase> emergencyCases = {
        {1, 3, 1, 1, "Waiting", "Severe chest pain"},
        {2, 2, 2, 2, "Waiting", "High fever"}
    };

    inline vector<HospitalLocation> hospitalLocations = {
        {1, "Reception", {2, 3}},
        {2, "Emergency Room", {1, 3}},
        {3, "Cardiology", {1, 2}}
    };

}

#endif
