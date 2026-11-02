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

// Department Hierarchy (General / N-ary Tree structure)
inline vector<Department> departments = {
    // Top-Level Divisions (parent = 0)
    {100, "Clinical Services", 0, "Comprehensive inpatient and outpatient medical care", "Tower A", "Dr. Tariq Mahmood", "Active"},
    {200, "Surgical Services", 0, "Specialized inpatient surgeries and operating theaters", "Tower B", "Dr. Kamran Ali", "Active"},
    {300, "Diagnostic & Support", 0, "Diagnostic imaging, laboratory, and clinical diagnostics", "Ground Floor", "Dr. Hina Aslam", "Active"},

    // Clinical Departments (parent = 100)
    {1, "Cardiology", 100, "Heart, coronary care and vascular health", "Block A, 2nd Floor", "Dr. Tariq Mahmood", "Active"},
    {2, "Neurology", 100, "Brain, spine and nervous system diagnostics", "Block A, 3rd Floor", "Dr. Zainab Bilal", "Active"},
    {4, "Pediatrics", 100, "Comprehensive child healthcare and neonatology", "Block A, 1st Floor", "Dr. Sarah Johnson", "Active"},
    {5, "Dermatology", 100, "Skin, hair and dermatological procedures", "OPD Block B, 1st Floor", "Dr. Bilal Ahmed", "Active"},
    {6, "General Medicine", 100, "Primary adult healthcare and internal medicine", "OPD Block B, Ground Floor", "Dr. Asad Rehman", "Active"},

    // Surgical Departments (parent = 200)
    {3, "Orthopedics", 200, "Musculoskeletal, joint, bone and trauma care", "Tower B, 4th Floor", "Dr. Kamran Ali", "Active"},
    {7, "General Surgery", 200, "General laparoscopic, abdominal and trauma surgery", "Tower B, 3rd Floor", "Dr. Usman Farooq", "Active"},

    // Specialized Clinical Sub-units (parent = 1)
    {10, "Interventional Cardiology", 1, "Catheterization, angioplasty and stenting unit", "Block A, Cath Lab", "Dr. Usman Farooq", "Active"},
    {11, "Electrophysiology & Echo Lab", 1, "Arrhythmia, pacemakers and echocardiogram testing", "Block A, 2nd Floor", "Dr. Zeeshan Haider", "Active"},

    // Specialized Sub-unit (parent = 2)
    {20, "Stroke Unit & Neuro-ICU", 2, "Acute neurological care and neuro-intensive care", "Block A, Neuro Wing", "Dr. Maryam Siddiqui", "Active"},

    // Specialized Sub-unit (parent = 4)
    {40, "Neonatology & Child Health", 4, "Newborn intensive care (NICU) and infant wellness", "Block A, NICU", "Dr. Nida Rashid", "Active"},

    // Specialized Sub-unit (parent = 3)
    {30, "Spine & Joint Replacement", 3, "Advanced robotic joint arthroplasty and spine clinic", "Tower B, Joint Wing", "Dr. Kamran Ali", "Active"}
};

// Doctors with department associations and roster details
inline vector<Doctor> doctors = {
    {1, "Dr. Ahmed", "Cardiologist", 1, {"09:00", "10:00", "11:00"}, 2500.0,
     "0301-1111222", "dr.ahmed@carepoint.pk", "Room 108 - CCU Wing", "MBBS, FCPS (Cardiology)", 12, "Available"},

    {2, "Dr. Sara", "Neurologist", 2, {"11:00", "12:00", "14:00"}, 3000.0,
     "0302-2222333", "dr.sara@carepoint.pk", "Room 312 - Neuro Wing", "MBBS, MRCP (Neurology)", 10, "Available"},

    {3, "Dr. Usman", "Orthopedic", 3, {"13:00", "14:00", "15:00"}, 2800.0,
     "0303-3333444", "dr.usman@carepoint.pk", "Room 402 - Joint OPD", "MBBS, FRCS (Orthopedics)", 14, "In Surgery"},

    {4, "Dr. Sarah Johnson", "Pediatrician", 4, {"09:30", "10:30", "11:30"}, 2500.0,
     "0301-5551234", "sarah.johnson@carepoint.pk", "Room 204 - Child Health", "MBBS, FCPS (Pediatrics)", 10, "Available"},

    {5, "Dr. Tariq Mahmood", "Cardiologist", 1, {"10:00", "12:00", "16:00"}, 4000.0,
     "0302-5552345", "tariq.mahmood@carepoint.pk", "Room 108 - CCU Wing", "MBBS, MRCP (Cardiology), FACC", 16, "Available"},

    {6, "Dr. Zainab Bilal", "Neurologist", 2, {"10:00", "11:30", "14:30"}, 4500.0,
     "0303-5553456", "zainab.bilal@carepoint.pk", "Room 312 - Neuro Wing", "MBBS, FCPS (Neurology)", 12, "In Surgery"},

    {7, "Dr. Bilal Ahmed", "Dermatologist", 5, {"11:00", "12:30", "15:00"}, 3000.0,
     "0304-5554567", "bilal.ahmed@carepoint.pk", "Room 105 - Derma Center", "MBBS, MCPS, Dip Derm (UK)", 8, "Available"},

    {8, "Dr. Fatima Noor", "Pediatrician", 4, {"09:00", "10:00"}, 2800.0,
     "0305-5555678", "fatima.noor@carepoint.pk", "Room 208 - Child Health Unit", "MBBS, DCH, FCPS", 11, "On Leave"},

    {9, "Dr. Usman Farooq", "Cardiologist", 10, {"08:30", "11:00", "13:30"}, 5000.0,
     "0306-5556789", "usman.farooq@carepoint.pk", "Room 114 - Cath Lab Suite", "MBBS, FCPS (Interventional Card)", 18, "In Surgery"},

    {10, "Dr. Maryam Siddiqui", "Neurologist", 20, {"10:00", "12:00", "14:00"}, 3800.0,
     "0307-5557890", "maryam.siddiqui@carepoint.pk", "Room 316 - Neuro Diagnostic", "MBBS, FCPS, FEBN", 9, "Available"},

    {11, "Dr. Kamran Ali", "Orthopedic Surgeon", 30, {"09:00", "11:00", "15:00"}, 3500.0,
     "0308-5558901", "kamran.ali@carepoint.pk", "Room 402 - Joint & Spine OPD", "MBBS, FRCS (Tr & Orth)", 14, "Available"},

    {12, "Dr. Asad Rehman", "General Physician", 6, {"08:00", "10:00", "12:00"}, 2000.0,
     "0310-5550123", "asad.rehman@carepoint.pk", "Room 101 - Primary Care", "MBBS, MRCP (UK)", 15, "Available"}
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
