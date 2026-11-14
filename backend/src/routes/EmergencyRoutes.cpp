#include "../../include/Routes.h"
#include "../../include/services/EmergencyService.h"
#include "../../include/libraries/json.hpp"

using json = nlohmann::json;
using namespace std;

static json caseToJson(const EmergencyCase& c)
{
    json item;
    item["id"] = c.id;
    item["patientId"] = c.patientId;
    item["patientName"] = EmergencyService::getPatientName(c.patientId);
    item["triageLevel"] = c.triageLevel;
    item["arrivalOrder"] = c.arrivalOrder;
    item["status"] = c.status;
    item["notes"] = c.notes;
    return item;
}

static void sendError(httplib::Response& res, int statusCode, const string& message)
{
    json error;
    error["message"] = message;
    res.status = statusCode;
    res.set_content(error.dump(4), "application/json");
}

void registerEmergencyRoutes(httplib::Server& server)
{
    // GET all cases in priority order
    server.Get("/api/emergency", [](const httplib::Request&, httplib::Response& res)
    {
        json result = json::array();

        for (const auto& c : EmergencyService::getPriorityOrder())
            result.push_back(caseToJson(c));

        res.set_content(result.dump(4), "application/json");
    });

    // GET one case by id
    server.Get(R"(/api/emergency/(\d+))", [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        EmergencyCase* c = EmergencyService::findCase(id);

        if (c == nullptr)
        {
            sendError(res, 404, "Emergency case not found");
            return;
        }

        res.set_content(caseToJson(*c).dump(4), "application/json");
    });

    // POST add a new case
    server.Post("/api/emergency", [](const httplib::Request& req, httplib::Response& res)
    {
        json body = json::parse(req.body, nullptr, false);

        if (body.is_discarded() || !body.is_object())
        {
            sendError(res, 400, "Invalid JSON body");
            return;
        }

        if (!body.contains("patientId") || !body["patientId"].is_number_integer() ||
            !body.contains("triageLevel") || !body["triageLevel"].is_number_integer() ||
            !body.contains("notes") || !body["notes"].is_string())
        {
            sendError(res, 400, "patientId, triageLevel and notes are required");
            return;
        }

        int patientId = body["patientId"];
        int triageLevel = body["triageLevel"];
        string notes = body["notes"];

        if (triageLevel < 1 || triageLevel > 5)
        {
            sendError(res, 400, "triageLevel must be between 1 and 5");
            return;
        }

        if (notes.empty())
        {
            sendError(res, 400, "notes cannot be empty");
            return;
        }

        if (!EmergencyService::patientExists(patientId))
        {
            sendError(res, 404, "Patient not found");
            return;
        }

        EmergencyCase created;
        EmergencyService::createCase(patientId, triageLevel, notes, created);

        res.status = 201;
        res.set_content(caseToJson(created).dump(4), "application/json");
    });

    // POST treat the most urgent waiting case
    server.Post("/api/emergency/treat-next", [](const httplib::Request&, httplib::Response& res)
    {
        EmergencyCase treated;

        if (!EmergencyService::treatNext(treated))
        {
            sendError(res, 404, "No waiting emergency cases");
            return;
        }

        res.set_content(caseToJson(treated).dump(4), "application/json");
    });

    // PATCH change status: Waiting, In Treatment or Completed
    server.Patch(R"(/api/emergency/(\d+)/status)", [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        json body = json::parse(req.body, nullptr, false);

        if (body.is_discarded() || !body.is_object() ||
            !body.contains("status") || !body["status"].is_string())
        {
            sendError(res, 400, "status is required");
            return;
        }

        string status = body["status"];

        if (!EmergencyService::isValidStatus(status))
        {
            sendError(res, 400, "status must be Waiting, In Treatment or Completed");
            return;
        }

        if (!EmergencyService::updateStatus(id, status))
        {
            sendError(res, 404, "Emergency case not found");
            return;
        }

        res.set_content(caseToJson(*EmergencyService::findCase(id)).dump(4), "application/json");
    });
}