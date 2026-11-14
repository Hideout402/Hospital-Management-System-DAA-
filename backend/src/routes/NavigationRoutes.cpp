#include "../../include/Routes.h"
#include "../../include/services/NavigationService.h"
#include "../../include/libraries/json.hpp"

using json = nlohmann::json;
using namespace std;

static void sendError(httplib::Response& res, int statusCode, const string& message)
{
    json error;
    error["message"] = message;
    res.status = statusCode;
    res.set_content(error.dump(4), "application/json");
}

// Reads a positive whole number from the URL, e.g. ?from=3
static bool readIntParam(const httplib::Request& req, const string& name, int& value)
{
    if (!req.has_param(name))
        return false;

    string text = req.get_param_value(name);

    if (text.empty() || text.size() > 6)
        return false;

    for (char ch : text)
    {
        if (ch < '0' || ch > '9')
            return false;
    }

    value = stoi(text);
    return true;
}

static json idsToNames(const vector<int>& ids)
{
    json names = json::array();
    for (int id : ids)
        names.push_back(NavigationService::getLocationName(id));
    return names;
}

void registerNavigationRoutes(httplib::Server& server)
{
    // GET all locations with their corridors
    server.Get("/api/navigation/locations", [](const httplib::Request&, httplib::Response& res)
    {
        json result = json::array();

        for (const auto& l : NavigationService::getLocations())
        {
            result.push_back({
                {"id", l.id},
                {"name", l.name},
                {"connectedLocationIds", l.connectedLocationIds}
            });
        }

        res.set_content(result.dump(4), "application/json");
    });

    // GET shortest route with BFS: /api/navigation/route?from=1&to=8
    server.Get("/api/navigation/route", [](const httplib::Request& req, httplib::Response& res)
    {
        int from = 0;
        int to = 0;

        if (!readIntParam(req, "from", from) || !readIntParam(req, "to", to))
        {
            sendError(res, 400, "from and to must be valid location ids");
            return;
        }

        if (!NavigationService::locationExists(from) || !NavigationService::locationExists(to))
        {
            sendError(res, 404, "Location not found");
            return;
        }

        vector<int> path = NavigationService::findRoute(from, to);

        if (path.empty())
        {
            sendError(res, 404, "No route found between these locations");
            return;
        }

        json result;
        result["from"] = from;
        result["to"] = to;
        result["corridors"] = (int)path.size() - 1;
        result["path"] = path;
        result["pathNames"] = idsToNames(path);

        res.set_content(result.dump(4), "application/json");
    });

    // GET locations reachable with DFS: /api/navigation/reachable?from=1
    server.Get("/api/navigation/reachable", [](const httplib::Request& req, httplib::Response& res)
    {
        int from = 0;

        if (!readIntParam(req, "from", from))
        {
            sendError(res, 400, "from must be a valid location id");
            return;
        }

        if (!NavigationService::locationExists(from))
        {
            sendError(res, 404, "Location not found");
            return;
        }

        vector<int> order = NavigationService::getReachable(from);

        json result;
        result["from"] = from;
        result["count"] = (int)order.size();
        result["order"] = order;
        result["orderNames"] = idsToNames(order);

        res.set_content(result.dump(4), "application/json");
    });
}