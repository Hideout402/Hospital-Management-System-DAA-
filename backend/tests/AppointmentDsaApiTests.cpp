#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "../include/Routes.h"
#include "../include/libraries/json.hpp"

using json = nlohmann::json;
using namespace std;

int main(int argc, char** argv)
{
    httplib::Server server;
    registerAppointmentRoutes(server);
    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });
    server.Options(R"(.*)", [](const httplib::Request&, httplib::Response& response) {
        response.status = 200;
    });

    if (argc > 1 && string(argv[1]) == "--serve")
    {
        if (!server.bind_to_port("127.0.0.1", 18080))
        {
            cerr << "Could not bind the appointment DSA test server to port 18080." << endl;
            return 1;
        }
        cout << "Appointment DSA test API listening at http://127.0.0.1:18080" << endl;
        return server.listen_after_bind() ? 0 : 1;
    }

    const int port = server.bind_to_any_port("127.0.0.1");
    assert(port > 0);
    thread serverThread([&server]() { server.listen_after_bind(); });
    httplib::Client client("127.0.0.1", port);
    client.set_connection_timeout(2, 0);

    const auto get = [&client](const string& path) {
        auto response = client.Get(path);
        assert(response != nullptr);
        return response;
    };
    const auto post = [&client](const string& path, const json& body) {
        auto response = client.Post(path, body.dump(), "application/json");
        assert(response != nullptr);
        return response;
    };
    const auto remove = [&client](const string& path) {
        auto response = client.Delete(path);
        assert(response != nullptr);
        return response;
    };

    const json options = json::parse(get("/api/appointments/options")->body);
    assert(!options["patients"].empty() && !options["doctors"].empty());
    const int patientId = options["patients"][0]["id"].get<int>();
    const int doctorId = options["doctors"][0]["id"].get<int>();
    const json initialAppointments = json::parse(get("/api/appointments")->body);
    vector<int> appointmentIds;
    for (int day = 1; day <= 11; ++day)
    {
        const string dayString = day < 10 ? "0" + to_string(day) : to_string(day);
        const json draft = {
            {"patientId", patientId},
            {"doctorId", doctorId},
            {"date", "2099-01-" + dayString},
            {"time", "09:00"},
            {"duration", 30},
            {"type", "Consultation"},
            {"reason", "DSA API integration test"},
            {"notes", ""},
            {"status", "Scheduled"}
        };
        const auto created = post("/api/appointments", draft);
        assert(created->status == 201);
        const json createdPayload = json::parse(created->body);
        assert(createdPayload["success"].get<bool>());
        assert(createdPayload["appointment"].contains("patientId"));
        assert(createdPayload["appointment"].contains("doctorId"));
        assert(createdPayload["appointment"].contains("date"));
        appointmentIds.push_back(createdPayload["appointment"]["id"].get<int>());
    }
    const json afterCreate = json::parse(get("/api/appointments")->body);
    assert(afterCreate.size() == initialAppointments.size() + appointmentIds.size());
    const json filtered = json::parse(get(
        "/api/appointments?patientId=" + to_string(patientId) +
        "&doctorId=" + to_string(doctorId) + "&status=Scheduled")->body);
    assert(filtered.size() >= appointmentIds.size());
    const json appointmentById = json::parse(get(
        "/api/appointments/" + to_string(appointmentIds[0]))->body);
    assert(appointmentById["id"].get<int>() == appointmentIds[0]);
    cout << "PASS: Existing appointment list, filters, and get-by-ID response contracts" << endl;

    for (const string& algorithm : {"bubble", "merge", "quick"})
    {
        const json sorted = json::parse(get(
            "/api/appointments/dsa/sort?algorithm=" + algorithm + "&by=dateTime")->body);
        assert(sorted["appointments"].size() >= appointmentIds.size());
        const auto& records = sorted["appointments"];
        for (size_t i = 1; i < records.size(); ++i)
        {
            const string previous =
                records[i - 1]["date"].get<string>() + " " +
                records[i - 1]["time"].get<string>();
            const string current =
                records[i]["date"].get<string>() + " " +
                records[i]["time"].get<string>();
            assert(previous <= current);
        }
    }
    cout << "PASS: Bubble, Merge, and Quick Sort API endpoints sort live appointment records" << endl;

    const json idSorted = json::parse(
        get("/api/appointments/dsa/sort?algorithm=merge&by=id")->body);
    for (size_t i = 1; i < idSorted["appointments"].size(); ++i)
        assert(idSorted["appointments"][i - 1]["id"].get<int>() <=
            idSorted["appointments"][i]["id"].get<int>());
    cout << "PASS: Appointment records can also be sorted by ID" << endl;

    const json found = json::parse(get(
        "/api/appointments/dsa/search?id=" + to_string(appointmentIds[4]))->body);
    assert(found["found"].get<bool>());
    assert(found["appointment"]["id"].get<int>() == appointmentIds[4]);
    const json missing = json::parse(get(
        "/api/appointments/dsa/search?id=999999")->body);
    assert(!missing["found"].get<bool>());
    cout << "PASS: Binary Search finds existing and reports missing appointments" << endl;

    assert(json::parse(get("/api/appointments/dsa/queue")->body)["empty"].get<bool>());
    assert(post("/api/appointments/dsa/queue",
        {{"appointmentId", appointmentIds[0]}})->status == 200);
    assert(post("/api/appointments/dsa/queue",
        {{"appointmentId", appointmentIds[1]}})->status == 200);
    const json checkInQueue = json::parse(get("/api/appointments/dsa/queue")->body);
    assert(checkInQueue["appointmentIds"][0].get<int>() == appointmentIds[0]);
    assert(checkInQueue["appointmentIds"][1].get<int>() == appointmentIds[1]);
    const json processedFirst = json::parse(remove("/api/appointments/dsa/queue")->body);
    const json processedSecond = json::parse(remove("/api/appointments/dsa/queue")->body);
    assert(processedFirst["appointmentId"].get<int>() == appointmentIds[0]);
    assert(processedSecond["appointmentId"].get<int>() == appointmentIds[1]);
    assert(remove("/api/appointments/dsa/queue")->status == 409);
    cout << "PASS: Normal appointment queue is FIFO and empty dequeue is handled" << endl;

    for (int i = 0; i < 10; ++i)
        assert(post("/api/appointments/dsa/circular-queue",
            {{"appointmentId", appointmentIds[i]}})->status == 200);
    assert(json::parse(get("/api/appointments/dsa/circular-queue")->body)["full"].get<bool>());
    assert(post("/api/appointments/dsa/circular-queue",
        {{"appointmentId", appointmentIds[10]}})->status == 409);
    assert(json::parse(remove("/api/appointments/dsa/circular-queue")->body)
        ["appointmentId"].get<int>() == appointmentIds[0]);
    assert(post("/api/appointments/dsa/circular-queue",
        {{"appointmentId", appointmentIds[10]}})->status == 200);

    const json wrapped = json::parse(get("/api/appointments/dsa/circular-queue")->body);
    assert(wrapped["appointmentIds"].size() == 10);
    assert(wrapped["appointmentIds"][0].get<int>() == appointmentIds[1]);
    assert(post("/api/appointments/dsa/circular-queue/rotate", json::object())->status == 200);
    const json rotated = json::parse(get("/api/appointments/dsa/circular-queue")->body);
    assert(rotated["appointmentIds"][0].get<int>() == appointmentIds[2]);
    while (!json::parse(get("/api/appointments/dsa/circular-queue")->body)["empty"].get<bool>())
        remove("/api/appointments/dsa/circular-queue");
    assert(remove("/api/appointments/dsa/circular-queue")->status == 409);
    cout << "PASS: Circular appointment queue enforces capacity, wraps, rotates, and handles empty" << endl;

    const json history = json::parse(get("/api/appointments/dsa/history")->body);
    assert(!history.empty());
    const json peeked = json::parse(get("/api/appointments/dsa/history/peek")->body);
    assert(peeked["appointmentId"].get<int>() == history[0]["appointmentId"].get<int>());
    const json popped = json::parse(remove("/api/appointments/dsa/history")->body);
    assert(popped["action"]["appointmentId"].get<int>() ==
        history[0]["appointmentId"].get<int>());

    const json rescheduleDraft = {
        {"patientId", patientId},
        {"doctorId", doctorId},
        {"date", "2099-01-01"},
        {"time", "10:00"},
        {"duration", 30},
        {"type", "Consultation"},
        {"reason", "DSA API integration test"},
        {"notes", ""},
        {"status", "Scheduled"}
    };
    const auto rescheduled = client.Put(
        "/api/appointments/" + to_string(appointmentIds[0]),
        rescheduleDraft.dump(),
        "application/json");
    assert(rescheduled != nullptr && rescheduled->status == 200);
    const json updatedPayload = json::parse(rescheduled->body);
    assert(updatedPayload["success"].get<bool>());
    assert(updatedPayload["appointment"]["id"].get<int>() == appointmentIds[0]);
    assert(updatedPayload["appointment"]["time"] == "10:00");
    const json rescheduleHistory = json::parse(get("/api/appointments/dsa/history")->body);
    assert(rescheduleHistory[0]["action"] == "Rescheduled");
    assert(post("/api/appointments/dsa/queue",
        {{"appointmentId", appointmentIds[0]}})->status == 200);
    assert(post("/api/appointments/dsa/queue",
        {{"appointmentId", appointmentIds[2]}})->status == 200);
    assert(post("/api/appointments/dsa/circular-queue",
        {{"appointmentId", appointmentIds[0]}})->status == 200);
    assert(post("/api/appointments/dsa/circular-queue",
        {{"appointmentId", appointmentIds[2]}})->status == 200);

    const auto cancelled = client.Patch(
        "/api/appointments/" + to_string(appointmentIds[0]) + "/cancel", "", "");
    assert(cancelled != nullptr && cancelled->status == 200);
    const json cancelledPayload = json::parse(cancelled->body);
    assert(cancelledPayload["success"].get<bool>());
    assert(cancelledPayload["appointment"]["status"] == "Cancelled");
    const json normalAfterCancel = json::parse(get("/api/appointments/dsa/queue")->body);
    const json circularAfterCancel = json::parse(
        get("/api/appointments/dsa/circular-queue")->body);
    assert(normalAfterCancel["appointmentIds"].size() == 1);
    assert(normalAfterCancel["appointmentIds"][0].get<int>() == appointmentIds[2]);
    assert(circularAfterCancel["appointmentIds"].size() == 1);
    assert(circularAfterCancel["appointmentIds"][0].get<int>() == appointmentIds[2]);
    const json latestAction = json::parse(get("/api/appointments/dsa/history/peek")->body);
    assert(latestAction["action"] == "Cancelled");
    assert(latestAction["appointmentId"].get<int>() == appointmentIds[0]);
    cout << "PASS: Appointment action history exposes LIFO peek/pop and records rescheduling/cancellation" << endl;

    const json allAppointments = json::parse(get("/api/appointments")->body);
    for (size_t i = 0; i < appointmentIds.size(); ++i)
    {
        bool expectedStatus = false;
        for (const auto& appointment : allAppointments)
        {
            if (appointment["id"].get<int>() == appointmentIds[i])
                expectedStatus = appointment["status"] ==
                    (i == 0 ? "Cancelled" : "Scheduled");
        }
        assert(expectedStatus);
    }
    cout << "PASS: DSA operations preserve appointment data and remove cancelled IDs from queues" << endl;

    client.stop();
    server.stop();
    serverThread.join();
    return 0;
}
