#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include <string>
using namespace std;

struct Department {
    int id;
    string name;
    int parentDepartmentId;
};

#endif
