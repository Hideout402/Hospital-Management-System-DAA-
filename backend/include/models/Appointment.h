#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
using namespace std;

struct Appointment {
    int id;
    int patientId;
    int doctorId;
    string date;
    string time;
    string status;
    string type = "Consultation";
    int duration = 30;
    string reason;
    string notes;
};

#endif
