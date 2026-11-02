#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include <string>
#include <utility>

struct Department {
    int id = 0;
    std::string name;
    int parentDepartmentId = 0; // 0 indicates a top-level hospital department / division
    std::string description;
    std::string location;
    std::string headDoctor;
    std::string status = "Active";

    Department() = default;

    Department(int id, std::string name, int parentDepartmentId = 0,
               std::string description = "", std::string location = "",
               std::string headDoctor = "", std::string status = "Active")
        : id(id),
          name(std::move(name)),
          parentDepartmentId(parentDepartmentId),
          description(std::move(description)),
          location(std::move(location)),
          headDoctor(std::move(headDoctor)),
          status(std::move(status)) {}
};

#endif
