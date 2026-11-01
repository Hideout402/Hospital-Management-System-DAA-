#include <iostream>
#include "libraries/httplib.h"
#include "Routes.h"

using namespace std;
using namespace httplib;

int main()
{
    Server server;

    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    server.Options(R"(.*)", [](const Request&, Response& res)
    {
        res.status = 200;
    });

    server.Get("/api/health", [](const Request&, Response& res)
    {
        res.set_content(
            R"({"status":"success","message":"Hospital Management System Backend is running"})",
            "application/json"
        );
    });

    registerPatientRoutes(server);
    registerBillingRoutes(server);

    cout << "Hospital Management System Backend" << endl;
    cout << "Server running at http://localhost:8080" << endl;

    if (!server.listen("127.0.0.1", 8080))
    {
        cerr << "Failed to start server." << endl;
        return 1;
    }

    return 0;
}
