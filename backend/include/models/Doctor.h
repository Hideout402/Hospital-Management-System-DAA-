#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>
#include <vector>
#include <utility>

struct Doctor {
    int id = 0;
    std::string name;
    std::string specialty; // Also referred to as specialization
    int departmentId = 0;
    std::vector<std::string> availableSlots;
    double consultationFee = 0.0;
    std::string contact;   // Phone / contact information
    std::string email;
    std::string roomNumber;
    std::string qualification;
    int experienceYears = 0;
    std::string availability = "Available"; // "Available", "On Leave", "In Surgery"

    Doctor() = default;

    Doctor(int id, std::string name, std::string specialty, int departmentId,
           std::vector<std::string> availableSlots = {}, double consultationFee = 0.0,
           std::string contact = "", std::string email = "", std::string roomNumber = "",
           std::string qualification = "", int experienceYears = 0,
           std::string availability = "Available")
        : id(id),
          name(std::move(name)),
          specialty(std::move(specialty)),
          departmentId(departmentId),
          availableSlots(std::move(availableSlots)),
          consultationFee(consultationFee),
          contact(std::move(contact)),
          email(std::move(email)),
          roomNumber(std::move(roomNumber)),
          qualification(std::move(qualification)),
          experienceYears(experienceYears),
          availability(std::move(availability)) {}
};

#endif
