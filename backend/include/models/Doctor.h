#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>
#include <vector>
using namespace std;

struct Doctor {
    int id;
    string name;
    string specialty;
    int departmentId;
    vector<string> availableSlots;
    double consultationFee;
};

#endif
