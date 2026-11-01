#ifndef EMERGENCYCASE_H
#define EMERGENCYCASE_H

#include <string>
using namespace std;

struct EmergencyCase {
    int id;
    int patientId;
    int triageLevel;
    int arrivalOrder;
    string status;
    string notes;
};

#endif
