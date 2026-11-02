#include "../../include/Routes.h"
#include "../../include/services/AppointmentService.h"
#include "../../include/services/EAppointmentDsaService.h"
#include "../../include/libraries/json.hpp"
#include "../../include/dsa/EAppointmentSearching.h"
#include "../../include/dsa/EAppointmentSorting.h"
#include "../../include/dsa/Sorting.h"

#include <algorithm>
#include <cctype>
#include <exception>
#include <functional>
#include <stdexcept>
#include <string>

using json = nlohmann::json;
using namespace std;

namespace {

json appointmentToJson(const Appointment& appointment)
{
    return {
        {"id", appointment.id},
        {"patientId", appointment.patientId},
        {"doctorId", appointment.doctorId},
        {"date", appointment.date},
        {"time", appointment.time},
        {"duration", appointment.duration},
        {"type", appointment.type},
        {"reason", appointment.reason},
        {"notes", appointment.notes},
        {"status", appointment.status}
    };
}

void setError(httplib::Response& response, int status, const string& message)
{
    response.status = status;
    response.set_content(
        json({{"success", false}, {"message", message}}).dump(4),
        "application/json");
}

bool parsePositiveInt(const string& value, int& result)
{
    if (value.empty() ||
        !all_of(value.begin(), value.end(), [](unsigned char ch) {
            return isdigit(ch) != 0;
        }))
        return false;

    try
    {
        size_t parsed = 0;
        result = stoi(value, &parsed);
        return parsed == value.size() && result > 0;
    }
    catch (const exception&)
    {
        return false;
    }
}

Appointment appointmentFromJson(const json& body, int defaultDuration = 30)
{
    if (!body.is_object())
        throw invalid_argument("Request body must be a JSON object.");

    Appointment appointment{};
    appointment.id = body.value("id", 0);
    appointment.patientId = body.at("patientId").get<int>();
    appointment.doctorId = body.at("doctorId").get<int>();
    appointment.date = body.at("date").get<string>();
    appointment.time = body.at("time").get<string>();
    appointment.type = body.value("type", string("Consultation"));
    appointment.duration = body.value("duration", defaultDuration);
    appointment.reason = body.at("reason").get<string>();
    appointment.notes = body.value("notes", string());
    appointment.status = body.value("status", string("Scheduled"));
    return appointment;
}

json appointmentsToJson(const vector<Appointment>& appointments)
{
    json result = json::array();
    for (const auto& appointment : appointments)
        result.push_back(appointmentToJson(appointment));
    return result;
}

bool appointmentLess(const Appointment& left, const Appointment& right, const string& by)
{
    if (by == "id")
        return left.id < right.id;
    if (by == "patientId")
        return left.patientId < right.patientId;
    if (by == "doctorId")
        return left.doctorId < right.doctorId;
    if (by == "status")
        return left.status < right.status;
    return left.date + " " + left.time < right.date + " " + right.time;
}

bool parseAppointmentId(const json& body, int& appointmentId)
{
    if (!body.is_object() || !body.contains("appointmentId") ||
        !body["appointmentId"].is_number_integer())
        return false;
    appointmentId = body["appointmentId"].get<int>();
    return appointmentId > 0;
}

json actionToJson(const EAppointmentAction& action)
{
    return {
        {"appointmentId", action.appointmentId},
        {"action", action.action},
        {"details", action.details},
        {"timestamp", action.timestamp}
    };
}

json appointmentOptions()
{
    json patients = json::array();
    for (const auto& patient : DataStore::getPatients())
    {
        patients.push_back({
            {"id", patient.id},
            {"name", patient.name},
            {"age", patient.age},
            {"phone", patient.phone},
            {"gender", patient.gender}
        });
    }

    json doctors = json::array();
    for (const auto& doctor : DataStore::getDoctors())
    {
        string departmentName;
        for (const auto& department : DataStore::getDepartments())
        {
            if (department.id == doctor.departmentId)
            {
                departmentName = department.name;
                break;
            }
        }

        doctors.push_back({
            {"id", doctor.id},
            {"name", doctor.name},
            {"specialty", doctor.specialty},
            {"department", departmentName},
            {"fee", doctor.consultationFee}
        });
    }

    return {{"patients", patients}, {"doctors", doctors}};
}

} // namespace

void registerAppointmentRoutes(httplib::Server& server)
{
    server.Get("/api/appointments/options",
        [](const httplib::Request&, httplib::Response& response)
    {
        response.set_content(appointmentOptions().dump(4), "application/json");
    });

    server.Get("/api/appointments/dsa/sort",
        [](const httplib::Request& request, httplib::Response& response)
    {
        const string algorithm = request.has_param("algorithm")
            ? request.get_param_value("algorithm") : string();
        const string by = request.has_param("by")
            ? request.get_param_value("by") : string("dateTime");
        const bool descending = request.has_param("order") &&
            request.get_param_value("order") == "desc";

        if (algorithm != "bubble" && algorithm != "merge" && algorithm != "quick")
        {
            setError(response, 400, "algorithm must be bubble, merge, or quick.");
            return;
        }
        if (by != "dateTime" && by != "id" && by != "patientId" &&
            by != "doctorId" && by != "status")
        {
            setError(response, 400, "by must be dateTime, id, patientId, doctorId, or status.");
            return;
        }
        if (request.has_param("order") &&
            request.get_param_value("order") != "asc" &&
            request.get_param_value("order") != "desc")
        {
            setError(response, 400, "order must be asc or desc.");
            return;
        }

        vector<Appointment> sorted = AppointmentService::getAllAppointments();
        const function<bool(const Appointment&, const Appointment&)> less =
            [by, descending](const Appointment& left, const Appointment& right) {
                return descending
                    ? appointmentLess(right, left, by)
                    : appointmentLess(left, right, by);
            };

        if (algorithm == "bubble")
            bubbleSort(sorted, less);
        else if (algorithm == "merge")
            mergeSort(sorted, 0, static_cast<int>(sorted.size()) - 1, less);
        else
            quickSort(sorted, less);

        response.set_content(
            json({
                {"algorithm", algorithm},
                {"by", by},
                {"order", descending ? "desc" : "asc"},
                {"appointments", appointmentsToJson(sorted)}
            }).dump(4),
            "application/json");
    });

    server.Get("/api/appointments/dsa/search",
        [](const httplib::Request& request, httplib::Response& response)
    {
        int id = 0;
        if (!request.has_param("id") ||
            !parsePositiveInt(request.get_param_value("id"), id))
        {
            setError(response, 400, "id must be a positive integer.");
            return;
        }

        vector<Appointment> sorted = AppointmentService::getAllAppointments();
        quickSort(sorted, [](const Appointment& left, const Appointment& right) {
            return left.id < right.id;
        });
        const int index = appointmentBinarySearch(
            sorted, id, [](const Appointment& appointment) { return appointment.id; });

        if (index < 0)
        {
            response.set_content(
                json({{"found", false}, {"message", "No appointment found for that ID."}}).dump(4),
                "application/json");
            return;
        }

        response.set_content(
            json({
                {"found", true},
                {"message", "Appointment found."},
                {"appointment", appointmentToJson(sorted[static_cast<size_t>(index)])}
            }).dump(4),
            "application/json");
    });

    server.Get("/api/appointments/dsa/queue",
        [](const httplib::Request&, httplib::Response& response)
    {
        const vector<int> ids = EAppointmentDsaService::getCheckInQueue();
        response.set_content(
            json({
                {"appointmentIds", ids},
                {"front", ids.empty() ? json(nullptr) : json(ids.front())},
                {"empty", ids.empty()}
            }).dump(4),
            "application/json");
    });

    server.Post("/api/appointments/dsa/queue",
        [](const httplib::Request& request, httplib::Response& response)
    {
        try
        {
            const json body = json::parse(request.body);
            int id = 0;
            if (!parseAppointmentId(body, id))
            {
                setError(response, 400, "appointmentId must be a positive integer.");
                return;
            }
            Appointment* appointment = AppointmentService::findAppointment(id);
            if (appointment == nullptr)
            {
                setError(response, 404, "Appointment not found.");
                return;
            }
            if (appointment->status != "Scheduled")
            {
                setError(response, 400, "Only scheduled appointments can be checked in.");
                return;
            }
            if (!EAppointmentDsaService::enqueueCheckIn(id))
            {
                setError(response, 400, "Appointment is already in the check-in queue.");
                return;
            }
            EAppointmentDsaService::recordAction(id, "Checked in", "Added to the FIFO check-in queue.");
            response.set_content(
                json({{"success", true}, {"message", "Appointment added to check-in queue."}}).dump(4),
                "application/json");
        }
        catch (const exception& error)
        {
            setError(response, 400, error.what());
        }
    });

    server.Delete("/api/appointments/dsa/queue",
        [](const httplib::Request&, httplib::Response& response)
    {
        int id = 0;
        if (!EAppointmentDsaService::dequeueCheckIn(id))
        {
            setError(response, 409, "The check-in queue is empty.");
            return;
        }
        EAppointmentDsaService::recordAction(id, "Processed", "Removed from the FIFO check-in queue.");
        response.set_content(
            json({{"success", true}, {"message", "Next appointment processed FIFO."}, {"appointmentId", id}}).dump(4),
            "application/json");
    });

    server.Get("/api/appointments/dsa/circular-queue",
        [](const httplib::Request&, httplib::Response& response)
    {
        const vector<int> ids = EAppointmentDsaService::getWaitingRoomQueue();
        const int capacity = EAppointmentDsaService::getWaitingRoomCapacity();
        response.set_content(
            json({
                {"appointmentIds", ids},
                {"front", ids.empty() ? json(nullptr) : json(ids.front())},
                {"empty", ids.empty()},
                {"full", ids.size() >= static_cast<size_t>(capacity)},
                {"capacity", capacity}
            }).dump(4),
            "application/json");
    });

    server.Post("/api/appointments/dsa/circular-queue",
        [](const httplib::Request& request, httplib::Response& response)
    {
        try
        {
            const json body = json::parse(request.body);
            int id = 0;
            if (!parseAppointmentId(body, id))
            {
                setError(response, 400, "appointmentId must be a positive integer.");
                return;
            }
            Appointment* appointment = AppointmentService::findAppointment(id);
            if (appointment == nullptr)
            {
                setError(response, 404, "Appointment not found.");
                return;
            }
            if (appointment->status != "Scheduled")
            {
                setError(response, 400, "Only scheduled appointments can enter the waiting room.");
                return;
            }
            if (!EAppointmentDsaService::enqueueWaitingRoom(id))
            {
                const vector<int> waitingIds = EAppointmentDsaService::getWaitingRoomQueue();
                const bool alreadyQueued =
                    find(waitingIds.begin(), waitingIds.end(), id) != waitingIds.end();
                setError(response, 409, alreadyQueued
                    ? "Appointment is already in the circular waiting-room queue."
                    : "The circular waiting-room queue is full.");
                return;
            }
            EAppointmentDsaService::recordAction(
                id, "Added to waiting room", "Added to the circular waiting-room queue.");
            response.set_content(
                json({{"success", true}, {"message", "Appointment added to circular waiting-room queue."}}).dump(4),
                "application/json");
        }
        catch (const exception& error)
        {
            setError(response, 400, error.what());
        }
    });

    server.Delete("/api/appointments/dsa/circular-queue",
        [](const httplib::Request&, httplib::Response& response)
    {
        int id = 0;
        if (!EAppointmentDsaService::dequeueWaitingRoom(id))
        {
            setError(response, 409, "The circular waiting-room queue is empty.");
            return;
        }
        EAppointmentDsaService::recordAction(
            id, "Removed from waiting room", "Removed from the circular waiting-room queue.");
        response.set_content(
            json({{"success", true}, {"message", "Appointment removed from waiting room."}, {"appointmentId", id}}).dump(4),
            "application/json");
    });

    server.Post("/api/appointments/dsa/circular-queue/rotate",
        [](const httplib::Request&, httplib::Response& response)
    {
        int id = 0;
        if (!EAppointmentDsaService::rotateWaitingRoom(id))
        {
            setError(response, 409, "At least two appointments are required to rotate the circular queue.");
            return;
        }
        EAppointmentDsaService::recordAction(
            id, "Rotated in waiting room", "Moved from the front to the rear of the circular queue.");
        response.set_content(
            json({{"success", true}, {"message", "Front appointment moved to the rear."}, {"appointmentId", id}}).dump(4),
            "application/json");
    });

    server.Get("/api/appointments/dsa/history",
        [](const httplib::Request&, httplib::Response& response)
    {
        json actions = json::array();
        for (const auto& action : EAppointmentDsaService::getActionHistory())
            actions.push_back(actionToJson(action));
        response.set_content(actions.dump(4), "application/json");
    });

    server.Get("/api/appointments/dsa/history/peek",
        [](const httplib::Request&, httplib::Response& response)
    {
        EAppointmentAction action{};
        if (!EAppointmentDsaService::peekActionHistory(action))
        {
            setError(response, 409, "Appointment action history is empty.");
            return;
        }
        response.set_content(actionToJson(action).dump(4), "application/json");
    });

    server.Delete("/api/appointments/dsa/history",
        [](const httplib::Request&, httplib::Response& response)
    {
        EAppointmentAction action{};
        if (!EAppointmentDsaService::popActionHistory(action))
        {
            setError(response, 409, "Appointment action history is empty.");
            return;
        }
        response.set_content(
            json({{"success", true}, {"message", "Most recent history action popped."}, {"action", actionToJson(action)}}).dump(4),
            "application/json");
    });

    server.Get("/api/appointments",
        [](const httplib::Request& request, httplib::Response& response)
    {
        int patientId = 0;
        int doctorId = 0;
        if (request.has_param("patientId") &&
            !parsePositiveInt(request.get_param_value("patientId"), patientId))
        {
            setError(response, 400, "patientId must be a positive integer.");
            return;
        }
        if (request.has_param("doctorId") &&
            !parsePositiveInt(request.get_param_value("doctorId"), doctorId))
        {
            setError(response, 400, "doctorId must be a positive integer.");
            return;
        }

        const string status = request.has_param("status")
            ? request.get_param_value("status")
            : string();
        if (!status.empty() && status != "Scheduled" &&
            status != "Completed" && status != "Cancelled")
        {
            setError(response, 400, "status is invalid.");
            return;
        }

        vector<Appointment> appointments = AppointmentService::getAllAppointments();
        appointments.erase(
            remove_if(appointments.begin(), appointments.end(),
                [patientId, doctorId, &status](const Appointment& appointment) {
                    return (patientId > 0 && appointment.patientId != patientId) ||
                        (doctorId > 0 && appointment.doctorId != doctorId) ||
                        (!status.empty() && appointment.status != status);
                }),
            appointments.end());

        response.set_content(appointmentsToJson(appointments).dump(4), "application/json");
    });

    server.Get(R"(/api/appointments/(\d+))",
        [](const httplib::Request& request, httplib::Response& response)
    {
        int id = 0;
        if (!parsePositiveInt(request.matches[1], id))
        {
            setError(response, 400, "Appointment ID must be a positive integer.");
            return;
        }

        Appointment* appointment = AppointmentService::findAppointment(id);
        if (appointment == nullptr)
        {
            setError(response, 404, "Appointment not found.");
            return;
        }

        response.set_content(appointmentToJson(*appointment).dump(4), "application/json");
    });

    server.Post("/api/appointments",
        [](const httplib::Request& request, httplib::Response& response)
    {
        try
        {
            const json body = json::parse(request.body);
            if (body.is_object() && body.contains("id") &&
                body.at("id").get<int>() <= 0)
                throw invalid_argument("Appointment ID must be a positive integer.");

            Appointment appointment = appointmentFromJson(body);
            string error;
            int assignedId = 0;
            if (!AppointmentService::addAppointment(appointment, error, &assignedId))
            {
                const int status = error == "Appointment ID already exists." ||
                    error == "Doctor has an overlapping appointment." ? 409 : 400;
                setError(response, status, error);
                return;
            }

            response.status = 201;
            response.set_content(
                json({
                    {"success", true},
                    {"message", "Appointment created successfully."},
                    {"appointment", appointmentToJson(
                        *AppointmentService::findAppointment(assignedId))}
                }).dump(4),
                "application/json");
            EAppointmentDsaService::recordAction(
                assignedId, "Created", "Appointment scheduled.");
        }
        catch (const exception& error)
        {
            setError(response, 400, error.what());
        }
    });

    server.Put(R"(/api/appointments/(\d+))",
        [](const httplib::Request& request, httplib::Response& response)
    {
        int id = 0;
        if (!parsePositiveInt(request.matches[1], id))
        {
            setError(response, 400, "Appointment ID must be a positive integer.");
            return;
        }

        try
        {
            Appointment* existing = AppointmentService::findAppointment(id);
            if (existing == nullptr)
            {
                setError(response, 404, "Appointment not found.");
                return;
            }
            const Appointment previous = *existing;

            const json body = json::parse(request.body);
            Appointment updated = appointmentFromJson(body, existing->duration);
            if (body.contains("id") && updated.id != id)
            {
                setError(response, 400, "Appointment ID must match the request path.");
                return;
            }
            string error;
            if (!AppointmentService::updateAppointment(id, updated, error))
            {
                const int status = error == "Doctor has an overlapping appointment."
                    ? 409
                    : error == "Appointment not found." ? 404 : 400;
                setError(response, status, error);
                return;
            }
            const bool rescheduled =
                previous.date != updated.date || previous.time != updated.time;
            if (updated.status != "Scheduled")
                EAppointmentDsaService::removeAppointmentFromQueues(id);
            EAppointmentDsaService::recordAction(
                id,
                rescheduled ? "Rescheduled" :
                    previous.status != updated.status ? updated.status : "Updated",
                rescheduled
                    ? "Appointment moved to " + updated.date + " at " + updated.time + "."
                    : previous.status != updated.status
                        ? "Appointment status changed to " + updated.status + "."
                        : "Appointment details updated.");

            response.set_content(
                json({
                    {"success", true},
                    {"message", "Appointment updated successfully."},
                    {"appointment", appointmentToJson(
                        *AppointmentService::findAppointment(id))}
                }).dump(4),
                "application/json");
        }
        catch (const exception& error)
        {
            setError(response, 400, error.what());
        }
    });

    server.Patch(R"(/api/appointments/(\d+)/cancel)",
        [](const httplib::Request& request, httplib::Response& response)
    {
        int id = 0;
        if (!parsePositiveInt(request.matches[1], id))
        {
            setError(response, 400, "Appointment ID must be a positive integer.");
            return;
        }

        string error;
        if (!AppointmentService::cancelAppointment(id, error))
        {
            setError(response, 404, error);
            return;
        }
        EAppointmentDsaService::removeAppointmentFromQueues(id);
        EAppointmentDsaService::recordAction(id, "Cancelled", "Appointment cancelled.");

        response.set_content(
            json({
                {"success", true},
                {"message", "Appointment cancelled successfully."},
                {"appointment", appointmentToJson(
                    *AppointmentService::findAppointment(id))}
            }).dump(4),
            "application/json");
    });
}
