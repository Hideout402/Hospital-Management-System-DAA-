
#include "../../include/Routes.h"
#include "../../include/services/PatientService.h"
#include "../../include/libraries/json.hpp"

using json = nlohmann::json;
using namespace std;

void registerPatientRoutes(httplib::Server& server)
{
    // GET all patients
    server.Get("/api/patients", [](const httplib::Request&, httplib::Response& res)
    {
        json result = json::array();

        for (const auto& p : PatientService::getAllPatients())
        {
            result.push_back({
                {"id", p.id},
                {"name", p.name},
                {"age", p.age},
                {"gender", p.gender},
                {"phone", p.phone},
                {"address", p.address},
                {"department", p.department},
                {"status", p.status},
                {"visitHistory", p.visitHistory}
            });
        }

        res.set_content(result.dump(4), "application/json");
    });

    // GET patient by ID
    server.Get(R"(/api/patients/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        Patient* p = PatientService::findPatient(id);

        if (!p)
        {
            res.status = 404;
            res.set_content(
                R"({"message":"Patient not found"})",
                "application/json"
            );
            return;
        }

        json result = {
            {"id", p->id},
            {"name", p->name},
            {"age", p->age},
            {"gender", p->gender},
            {"phone", p->phone},
            {"address", p->address},
            {"department", p->department},
            {"status", p->status},
            {"visitHistory", p->visitHistory}
        };

        res.set_content(result.dump(4), "application/json");
    });

    // ADD patient
    server.Post("/api/patients",
        [](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            json body = json::parse(req.body);

            Patient p;
            p.id = body.at("id").get<int>();
            p.name = body.at("name").get<string>();
            p.age = body.at("age").get<int>();
            p.gender = body.value("gender", "Other");
            p.phone = body.at("phone").get<string>();
            p.address = body.value("address", "");
            p.department = body.at("department").get<string>();
            p.status = body.value("status", "Active");
            p.visitHistory = body.value(
                "visitHistory", vector<string>{}
            );

            if (p.id <= 0 || p.name.empty() || p.age <= 0)
            {
                res.status = 400;
                res.set_content(
                    R"({"message":"Invalid patient information"})",
                    "application/json"
                );
                return;
            }

            if (!PatientService::addPatient(p))
            {
                res.status = 409;
                res.set_content(
                    R"({"message":"Patient ID already exists"})",
                    "application/json"
                );
                return;
            }

            res.status = 201;
            res.set_content(
                R"({"message":"Patient added successfully"})",
                "application/json"
            );
        }
        catch (...)
        {
            res.status = 400;
            res.set_content(
                R"({"message":"Invalid JSON or missing fields"})",
                "application/json"
            );
        }
    });

    // UPDATE patient
    server.Put(R"(/api/patients/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            int id = stoi(req.matches[1]);
            json body = json::parse(req.body);

            Patient p;
            p.id = id;
            p.name = body.at("name").get<string>();
            p.age = body.at("age").get<int>();
            p.gender = body.value("gender", "Other");
            p.phone = body.at("phone").get<string>();
            p.address = body.value("address", "");
            p.department = body.at("department").get<string>();
            p.status = body.value("status", "Active");
            p.visitHistory = body.value(
                "visitHistory", vector<string>{}
            );

            if (p.name.empty() || p.age <= 0)
            {
                res.status = 400;
                res.set_content(
                    R"({"message":"Invalid patient information"})",
                    "application/json"
                );
                return;
            }

            if (!PatientService::updatePatient(p))
            {
                res.status = 404;
                res.set_content(
                    R"({"message":"Patient not found"})",
                    "application/json"
                );
                return;
            }

            res.set_content(
                R"({"message":"Patient updated successfully"})",
                "application/json"
            );
        }
        catch (...)
        {
            res.status = 400;
            res.set_content(
                R"({"message":"Invalid JSON or missing fields"})",
                "application/json"
            );
        }
    });

    // DELETE patient
    server.Delete(R"(/api/patients/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);

        if (!PatientService::deletePatient(id))
        {
            res.status = 404;
            res.set_content(
                R"({"message":"Patient not found"})",
                "application/json"
            );
            return;
        }

        res.set_content(
            R"({"message":"Patient deleted successfully"})",
            "application/json"
        );
    });

    // Linked-list-based patient visit history traversal
    server.Get(R"(/api/patients/(\d+)/history)",
        [](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            int id = stoi(req.matches[1]);
            Patient* patient = PatientService::findPatient(id);

            if (!patient)
            {
                res.status = 404;
                res.set_content(
                    R"({"success":false,"message":"Patient not found"})",
                    "application/json"
                );
                return;
            }

            vector<string> history;
            if (!PatientService::getVisitHistoryTraversal(id, history))
            {
                res.status = 500;
                res.set_content(
                    R"({"success":false,"message":"Unable to load visit history"})",
                    "application/json"
                );
                return;
            }

            json result = {
                {"success", true},
                {"patientId", patient->id},
                {"patientName", patient->name},
                {"visitHistory", history}
            };

            res.set_content(result.dump(4), "application/json");
        }
        catch (const exception&)
        {
            res.status = 400;
            res.set_content(
                R"({"success":false,"message":"Invalid patient history request"})",
                "application/json"
            );
        }
    });
}
