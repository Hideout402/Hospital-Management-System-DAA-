#ifndef APPOINTMENTSERVICE_H
#define APPOINTMENTSERVICE_H

#include <algorithm>
#include <ctime>
#include <string>
#include <vector>

#include "../data/DataStore.h"

class AppointmentService
{
private:
    static bool isLeapYear(int year)
    {
        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    }

    static bool isValidDate(const string& date)
    {
        if (date.size() != 10 || date[4] != '-' || date[7] != '-')
            return false;

        for (size_t i = 0; i < date.size(); ++i)
        {
            if (i != 4 && i != 7 && (date[i] < '0' || date[i] > '9'))
                return false;
        }

        int year = stoi(date.substr(0, 4));
        int month = stoi(date.substr(5, 2));
        int day = stoi(date.substr(8, 2));

        if (year < 1 || month < 1 || month > 12)
            return false;

        const int daysInMonth[] = {
            31, isLeapYear(year) ? 29 : 28, 31, 30, 31, 30,
            31, 31, 30, 31, 30, 31
        };
        return day >= 1 && day <= daysInMonth[month - 1];
    }

    static bool isValidTime(const string& time)
    {
        return time.size() == 5 &&
            time[2] == ':' &&
            time[0] >= '0' && time[0] <= '2' &&
            time[1] >= '0' && time[1] <= '9' &&
            time[3] >= '0' && time[3] <= '5' &&
            time[4] >= '0' && time[4] <= '9' &&
            !(time[0] == '2' && time[1] > '3');
    }

    static int timeToMinutes(const string& time)
    {
        return stoi(time.substr(0, 2)) * 60 + stoi(time.substr(3, 2));
    }

    static bool appointmentIsInPast(const string& date, const string& appointmentTime)
    {
        const time_t now = time(nullptr);
        tm currentDate{};
#ifdef _WIN32
        localtime_s(&currentDate, &now);
#else
        localtime_r(&now, &currentDate);
#endif
        char currentDateTime[17];
        strftime(currentDateTime, sizeof(currentDateTime), "%Y-%m-%d %H:%M", &currentDate);
        return date + " " + appointmentTime < currentDateTime;
    }

    static bool hasPatient(int id)
    {
        for (const auto& patient : DataStore::getPatients())
        {
            if (patient.id == id)
                return true;
        }
        return false;
    }

    static bool hasDoctor(int id)
    {
        for (const auto& doctor : DataStore::getDoctors())
        {
            if (doctor.id == id)
                return true;
        }
        return false;
    }

public:
    static vector<Appointment>& getAllAppointments()
    {
        return DataStore::getAppointments();
    }

    static Appointment* findAppointment(int id)
    {
        for (auto& appointment : DataStore::getAppointments())
        {
            if (appointment.id == id)
                return &appointment;
        }

        return nullptr;
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

    static vector<Appointment> getDoctorAppointments(int doctorId)
    {
        vector<Appointment> result;
        for (const auto& appointment : DataStore::getAppointments())
        {
            if (appointment.doctorId == doctorId)
                result.push_back(appointment);
        }
        return result;
    }

    static vector<Appointment> getAppointmentsByStatus(const string& status)
    {
        vector<Appointment> result;
        for (const auto& appointment : DataStore::getAppointments())
        {
            if (appointment.status == status)
                result.push_back(appointment);
        }
        return result;
    }

    static bool isSlotAvailable(
        int doctorId,
        const string& date,
        const string& time,
        int duration = 30,
        int excludedId = 0)
    {
        if (!isValidDate(date) || !isValidTime(time) || duration <= 0)
            return false;

        const int start = timeToMinutes(time);
        const int end = start + duration;

        for (const auto& appointment : DataStore::getAppointments())
        {
            if (appointment.id == excludedId ||
                appointment.doctorId != doctorId ||
                appointment.date != date ||
                appointment.status == "Cancelled" ||
                !isValidTime(appointment.time) ||
                appointment.duration <= 0)
                continue;

            const int otherStart = timeToMinutes(appointment.time);
            const int otherEnd = otherStart + appointment.duration;
            if (start < otherEnd && otherStart < end)
                return false;
        }

        return true;
    }

    static bool validateAppointment(
        const Appointment& appointment,
        string& error,
        bool requireFutureDate = true,
        int excludedId = 0)
    {
        error.clear();

        if (appointment.patientId <= 0 || !hasPatient(appointment.patientId))
        {
            error = "Patient not found.";
            return false;
        }

        if (appointment.doctorId <= 0 || !hasDoctor(appointment.doctorId))
        {
            error = "Doctor not found.";
            return false;
        }

        if (!isValidDate(appointment.date))
        {
            error = "Date must be a valid YYYY-MM-DD date.";
            return false;
        }

        if (requireFutureDate && appointmentIsInPast(appointment.date, appointment.time))
        {
            error = "New appointments cannot be booked in the past.";
            return false;
        }

        if (!isValidTime(appointment.time))
        {
            error = "Time must be a valid 24-hour HH:MM value.";
            return false;
        }

        if (appointment.duration <= 0)
        {
            error = "Duration must be greater than zero.";
            return false;
        }

        if (appointment.type != "Consultation" &&
            appointment.type != "Follow-up" &&
            appointment.type != "Routine check-up")
        {
            error = "Appointment type is invalid.";
            return false;
        }

        if (appointment.status != "Scheduled" &&
            appointment.status != "Completed" &&
            appointment.status != "Cancelled")
        {
            error = "Appointment status is invalid.";
            return false;
        }

        if (appointment.reason.find_first_not_of(" \t\r\n") == string::npos)
        {
            error = "Reason for visit is required.";
            return false;
        }

        if (appointment.status != "Cancelled" &&
            !isSlotAvailable(
                appointment.doctorId,
                appointment.date,
                appointment.time,
                appointment.duration,
                excludedId))
        {
            error = "Doctor has an overlapping appointment.";
            return false;
        }

        return true;
    }

    static bool addAppointment(
        Appointment appointment,
        string& error,
        int* assignedId = nullptr)
    {
        auto& appointments = DataStore::getAppointments();

        if (appointment.id <= 0)
        {
            int nextId = 1;
            for (const auto& existing : appointments)
                nextId = max(nextId, existing.id + 1);
            appointment.id = nextId;
        }

        if (findAppointment(appointment.id) != nullptr)
        {
            error = "Appointment ID already exists.";
            return false;
        }

        if (!validateAppointment(appointment, error))
            return false;
        appointments.push_back(appointment);
        if (assignedId != nullptr)
            *assignedId = appointment.id;
        return true;
    }

    static bool addAppointment(Appointment appointment)
    {
        string error;
        return addAppointment(appointment, error);
    }

    static bool updateAppointment(int id, Appointment updated, string& error)
    {
        Appointment* existing = findAppointment(id);
        if (existing == nullptr)
        {
            error = "Appointment not found.";
            return false;
        }

        updated.id = id;
        if (!validateAppointment(updated, error, false, id))
            return false;

        *existing = updated;
        return true;
    }

    static bool cancelAppointment(int id, string& error)
    {
        Appointment* appointment = findAppointment(id);
        if (appointment == nullptr)
        {
            error = "Appointment not found.";
            return false;
        }

        appointment->status = "Cancelled";
        error.clear();
        return true;
    }

    static bool cancelAppointment(int id)
    {
        string error;
        return cancelAppointment(id, error);
    }
};

#endif
