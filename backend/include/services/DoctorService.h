#ifndef DOCTORSERVICE_H
#define DOCTORSERVICE_H

#include "../data/DataStore.h"

class DoctorService
{
public:
    static vector<Doctor>& getAllDoctors()
    {
        return DataStore::getDoctors();
    }

    static Doctor* findDoctor(int id)
    {
        for (auto& doctor : DataStore::getDoctors())
        {
            if (doctor.id == id)
                return &doctor;
        }
        return nullptr;
    }

    static bool addDoctor(Doctor doctor)
    {
        if (findDoctor(doctor.id) != nullptr)
            return false;

        DataStore::getDoctors().push_back(doctor);
        return true;
    }

    static bool deleteDoctor(int id)
    {
        auto& doctors = DataStore::getDoctors();

        for (int i = 0; i < doctors.size(); i++)
        {
            if (doctors[i].id == id)
            {
                doctors.erase(doctors.begin() + i);
                return true;
            }
        }
        return false;
    }

    static vector<Department>& getDepartments()
    {
        return DataStore::getDepartments();
    }

    static bool addDepartment(Department department)
    {
        for (const auto& d : DataStore::getDepartments())
        {
            if (d.id == department.id)
                return false;
        }

        DataStore::getDepartments().push_back(department);
        return true;
    }
};

#endif
