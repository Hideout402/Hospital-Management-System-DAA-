#include "../../include/Routes.h"
#include "../../include/services/DoctorService.h"
#include "../../include/services/DepartmentService.h"
#include "../../include/libraries/json.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

using json = nlohmann::json;
using namespace std;

namespace {

json doctorToJson(const Doctor& doc)
{
    string deptName = "Unknown";
    Department* dept = DepartmentService::findDepartment(doc.departmentId);
    if (dept)
    {
        deptName = dept->name;
    }

    return {
        {"id", doc.id},
        {"name", doc.name},
        {"specialty", doc.specialty},
        {"specialization", doc.specialty},
        {"departmentId", doc.departmentId},
        {"departmentName", deptName},
        {"contact", doc.contact},
        {"phone", doc.contact},
        {"email", doc.email},
        {"roomNumber", doc.roomNumber},
        {"qualification", doc.qualification},
        {"experienceYears", doc.experienceYears},
        {"availability", doc.availability},
        {"availableSlots", doc.availableSlots},
        {"consultationFee", doc.consultationFee},
        {"fee", doc.consultationFee}
    };
}

void setError(httplib::Response& res, int status, const string& message)
{
    res.status = status;
    res.set_content(
        json({{"success", false}, {"message", message}}).dump(4),
        "application/json"
    );
}

} // anonymous namespace

void registerDoctorRoutes(httplib::Server& server)
{
    // GET /api/doctors - List & filter doctors with multi-attribute sorting
    server.Get("/api/doctors", [](const httplib::Request& req, httplib::Response& res)
    {
        int departmentId = 0;
        if (req.has_param("departmentId"))
        {
            try {
                departmentId = stoi(req.get_param_value("departmentId"));
            } catch (...) {
                setError(res, 400, "Invalid departmentId parameter.");
                return;
            }
        }

        bool includeSubdepartments = false;
        if (req.has_param("includeSubdepartments"))
        {
            string val = req.get_param_value("includeSubdepartments");
            includeSubdepartments = (val == "true" || val == "1" || val == "yes");
        }

        string specialty = "";
        if (req.has_param("specialty"))
        {
            specialty = req.get_param_value("specialty");
        }
        else if (req.has_param("specialization"))
        {
            specialty = req.get_param_value("specialization");
        }

        string availability = "";
        if (req.has_param("availability"))
        {
            availability = req.get_param_value("availability");
        }
        else if (req.has_param("status"))
        {
            availability = req.get_param_value("status");
        }

        string search = "";
        if (req.has_param("search"))
        {
            search = req.get_param_value("search");
        }
        else if (req.has_param("q"))
        {
            search = req.get_param_value("q");
        }

        string sortBy = "id";
        if (req.has_param("sortBy"))
        {
            sortBy = req.get_param_value("sortBy");
        }

        string sortOrder = "asc";
        if (req.has_param("sortOrder"))
        {
            sortOrder = req.get_param_value("sortOrder");
        }
        else if (req.has_param("order"))
        {
            sortOrder = req.get_param_value("order");
        }

        // Apply filters
        vector<Doctor> doctors = DoctorService::filterDoctors(
            departmentId, specialty, availability, search, includeSubdepartments
        );

        // Apply sorting using Merge Sort / BST
        doctors = DoctorService::sortDoctors(doctors, sortBy, sortOrder);

        json result = json::array();
        for (const auto& doc : doctors)
        {
            result.push_back(doctorToJson(doc));
        }

        res.set_content(result.dump(4), "application/json");
    });

    // GET /api/doctors/roster - Doubly Linked List active roster (bi-directional traversal)
    server.Get("/api/doctors/roster", [](const httplib::Request& req, httplib::Response& res)
    {
        string direction = "forward";
        if (req.has_param("direction"))
        {
            direction = req.get_param_value("direction");
        }

        string availability = "";
        if (req.has_param("availability"))
        {
            availability = req.get_param_value("availability");
        }

        const bool forward = (direction != "backward" && direction != "reverse");
        vector<Doctor> roster = DoctorService::getRoster(forward, availability);

        json doctorsArray = json::array();
        for (const auto& doc : roster)
        {
            doctorsArray.push_back(doctorToJson(doc));
        }

        json responseObj = {
            {"success", true},
            {"dsaStructure", "Doubly Linked List"},
            {"traversalDirection", forward ? "Forward (Head -> Tail)" : "Backward (Tail -> Head)"},
            {"count", roster.size()},
            {"roster", doctorsArray}
        };

        res.set_content(responseObj.dump(4), "application/json");
    });

    // GET /api/doctors/dsa/bst-sorted - In-order traversal of Binary Search Tree
    server.Get("/api/doctors/dsa/bst-sorted", [](const httplib::Request&, httplib::Response& res)
    {
        vector<Doctor> sorted = DoctorService::getBstSortedDoctors();

        json doctorsArray = json::array();
        for (const auto& doc : sorted)
        {
            doctorsArray.push_back(doctorToJson(doc));
        }

        json responseObj = {
            {"success", true},
            {"dsaStructure", "Binary Search Tree (BST)"},
            {"traversal", "In-Order Traversal (Sorted by ID)"},
            {"count", sorted.size()},
            {"doctors", doctorsArray}
        };

        res.set_content(responseObj.dump(4), "application/json");
    });

    // GET /api/doctors/dsa/range - Range query using BST
    server.Get("/api/doctors/dsa/range", [](const httplib::Request& req, httplib::Response& res)
    {
        int minId = 1;
        int maxId = 1000;

        if (req.has_param("minId"))
        {
            try { minId = stoi(req.get_param_value("minId")); } catch (...) {}
        }
        if (req.has_param("maxId"))
        {
            try { maxId = stoi(req.get_param_value("maxId")); } catch (...) {}
        }

        vector<Doctor> inRange = DoctorService::getDoctorsInRange(minId, maxId);

        json doctorsArray = json::array();
        for (const auto& doc : inRange)
        {
            doctorsArray.push_back(doctorToJson(doc));
        }

        json responseObj = {
            {"success", true},
            {"dsaStructure", "Binary Search Tree (BST)"},
            {"operation", "Recursive Range Query"},
            {"minId", minId},
            {"maxId", maxId},
            {"count", inRange.size()},
            {"doctors", doctorsArray}
        };

        res.set_content(responseObj.dump(4), "application/json");
    });

    // GET /api/doctors/:id/next-on-call - Doubly Linked List shift rotation (next)
    server.Get(R"(/api/doctors/(\d+)/next-on-call)",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        const Doctor* nextDoc = DoctorService::getNextOnCallDoctor(id);

        if (!nextDoc)
        {
            setError(res, 404, "Doctor not found in active roster.");
            return;
        }

        json responseObj = {
            {"success", true},
            {"currentDoctorId", id},
            {"dsaPointer", "Doubly Linked List -> next"},
            {"nextDoctor", doctorToJson(*nextDoc)}
        };

        res.set_content(responseObj.dump(4), "application/json");
    });

    // GET /api/doctors/:id/prev-on-call - Doubly Linked List shift rotation (previous)
    server.Get(R"(/api/doctors/(\d+)/prev-on-call)",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        const Doctor* prevDoc = DoctorService::getPreviousOnCallDoctor(id);

        if (!prevDoc)
        {
            setError(res, 404, "Doctor not found in active roster.");
            return;
        }

        json responseObj = {
            {"success", true},
            {"currentDoctorId", id},
            {"dsaPointer", "Doubly Linked List -> prev"},
            {"prevDoctor", doctorToJson(*prevDoc)}
        };

        res.set_content(responseObj.dump(4), "application/json");
    });

    // GET /api/doctors/:id - Get single doctor details
    server.Get(R"(/api/doctors/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        Doctor* doc = DoctorService::findDoctor(id);

        if (!doc)
        {
            setError(res, 404, "Doctor not found.");
            return;
        }

        res.set_content(doctorToJson(*doc).dump(4), "application/json");
    });

    // POST /api/doctors - Add a new doctor profile
    server.Post("/api/doctors", [](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            json body = json::parse(req.body);

            Doctor doc;
            doc.id = body.value("id", 0);
            doc.name = body.at("name").get<string>();

            if (body.contains("specialty"))
            {
                doc.specialty = body.at("specialty").get<string>();
            }
            else if (body.contains("specialization"))
            {
                doc.specialty = body.at("specialization").get<string>();
            }
            else
            {
                setError(res, 400, "Missing required field: 'specialty' or 'specialization'.");
                return;
            }

            doc.departmentId = body.at("departmentId").get<int>();

            // Optional fields
            doc.contact = body.value("contact", body.value("phone", ""));
            doc.email = body.value("email", "");
            doc.roomNumber = body.value("roomNumber", "");
            doc.qualification = body.value("qualification", "MBBS");
            doc.experienceYears = body.value("experienceYears", 0);
            doc.availability = body.value("availability", "Available");
            doc.availableSlots = body.value("availableSlots", vector<string>{"09:00", "11:00", "14:00"});
            doc.consultationFee = body.value("consultationFee", body.value("fee", 2500.0));

            // Basic validation
            if (doc.name.empty() || doc.specialty.empty() || doc.departmentId <= 0)
            {
                setError(res, 400, "Invalid doctor information. Name, specialty, and departmentId are required.");
                return;
            }

            if (!DoctorService::addDoctor(doc))
            {
                setError(res, 409, "Failed to add doctor. Duplicate ID or assigned department does not exist.");
                return;
            }

            Doctor* created = DoctorService::findDoctor(doc.id);
            res.status = 201;
            res.set_content(
                json({
                    {"success", true},
                    {"message", "Doctor added successfully."},
                    {"doctor", created ? doctorToJson(*created) : doctorToJson(doc)}
                }).dump(4),
                "application/json"
            );
        }
        catch (const exception& e)
        {
            setError(res, 400, string("Invalid JSON payload: ") + e.what());
        }
    });

    // PUT /api/doctors/:id - Update existing doctor profile
    server.Put(R"(/api/doctors/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        try
        {
            json body = json::parse(req.body);

            Doctor updated;
            updated.id = id;
            updated.name = body.at("name").get<string>();

            if (body.contains("specialty"))
            {
                updated.specialty = body.at("specialty").get<string>();
            }
            else if (body.contains("specialization"))
            {
                updated.specialty = body.at("specialization").get<string>();
            }
            else
            {
                setError(res, 400, "Missing required field: 'specialty' or 'specialization'.");
                return;
            }

            updated.departmentId = body.value("departmentId", 0);
            updated.contact = body.value("contact", body.value("phone", ""));
            updated.email = body.value("email", "");
            updated.roomNumber = body.value("roomNumber", "");
            updated.qualification = body.value("qualification", "");
            updated.experienceYears = body.value("experienceYears", -1);
            updated.availability = body.value("availability", "");
            updated.availableSlots = body.value("availableSlots", vector<string>{});
            updated.consultationFee = body.value("consultationFee", body.value("fee", -1.0));

            string errorMsg;
            if (!DoctorService::updateDoctor(id, updated, errorMsg))
            {
                setError(res, errorMsg == "Doctor not found." ? 404 : 400, errorMsg);
                return;
            }

            Doctor* doc = DoctorService::findDoctor(id);
            res.set_content(
                json({
                    {"success", true},
                    {"message", "Doctor profile updated successfully."},
                    {"doctor", doctorToJson(*doc)}
                }).dump(4),
                "application/json"
            );
        }
        catch (const exception& e)
        {
            setError(res, 400, string("Invalid JSON payload: ") + e.what());
        }
    });

    // DELETE /api/doctors/:id - Remove doctor from directory & active roster
    server.Delete(R"(/api/doctors/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);

        if (!DoctorService::deleteDoctor(id))
        {
            setError(res, 404, "Doctor not found.");
            return;
        }

        res.set_content(
            json({
                {"success", true},
                {"message", "Doctor removed successfully."}
            }).dump(4),
            "application/json"
        );
    });
}
