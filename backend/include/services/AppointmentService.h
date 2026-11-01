#ifndef APPOINTMENTSERVICE_H
#define APPOINTMENTSERVICE_H

#include "../data/DataStore.h"

class AppointmentService
{
public:
    static vector<Appointment>& getAllAppointments()
    {
        return DataStore::getAppointments();
    }

    static bool isSlotAvailable(int doctorId, string date, string time)
    {
        for (const auto& appointment : DataStore::getAppointments())
        {
            if (appointment.doctorId == doctorId &&
                appointment.date == date &&
                appointment.time == time &&
                appointment.status != "Cancelled")
            {
                return false;
            }
        }

        return true;
    }

    static bool addAppointment(Appointment appointment)
    {
        auto& appointments = DataStore::getAppointments();

        for (const auto& a : appointments)
        {
            if (a.id == appointment.id)
                return false;
        }

        if (!isSlotAvailable(appointment.doctorId,
                            appointment.date,
                            appointment.time))
            return false;

        appointments.push_back(appointment);
        return true;
    }

    static bool cancelAppointment(int id)
    {
        for (auto& appointment : DataStore::getAppointments())
        {
            if (appointment.id == id)
            {
                appointment.status = "Cancelled";
                return true;
            }
        }

        return false;
    }

    static vector<Appointment> getPatientAppointments(int patientId)
    {
        vector<Appointment> result;

        for (const auto& appointment : DataStore::getAppointments())
        {
            if (appointment.patientId == patientId)
                result.push_back(appointment);
        }

        return result;
    }
};

#endif
