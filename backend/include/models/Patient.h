
#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <vector>
using namespace std;

struct Patient {
    int id;
    string name;
    int age;
    string gender;
    string phone;
    string address;
    string department;
    string status;
    vector<string> visitHistory;
};

#endif
