#ifndef HOSPITALLOCATION_H
#define HOSPITALLOCATION_H

#include <string>
#include <vector>
using namespace std;

struct HospitalLocation {
    int id;
    string name;
    vector<int> connectedLocationIds;
};

#endif
