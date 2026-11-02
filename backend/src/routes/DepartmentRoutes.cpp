#include "../../include/Routes.h"
#include "../../include/services/DepartmentService.h"
#include "../../include/services/DoctorService.h"
#include "../../include/libraries/json.hpp"

#include <string>
#include <vector>

using json = nlohmann::json;
using namespace std;

namespace {

json departmentToJson(const Department& dept)
{
    const auto& allDocs = DataStore::getDoctors();
    int doctorCount = 0;
    for (const auto& doc : allDocs)
    {
        if (doc.departmentId == dept.id)
        {
            doctorCount++;
        }
    }

    auto subdepts = DepartmentService::getSubdepartments(dept.id, false);

    return {
        {"id", dept.id},
        {"name", dept.name},
        {"parentDepartmentId", dept.parentDepartmentId},
        {"description", dept.description},
        {"location", dept.location},
        {"headDoctor", dept.headDoctor},
        {"status", dept.status},
        {"doctorCount", doctorCount},
        {"childDepartmentCount", subdepts.size()}
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

void registerDepartmentRoutes(httplib::Server& server)
{
    // GET /api/departments/hierarchy - Returns General Tree / N-ary Tree hierarchy as nested JSON
    server.Get("/api/departments/hierarchy", [](const httplib::Request&, httplib::Response& res)
    {
        json hierarchy = DepartmentService::getHierarchyJson();
        json responseObj = {
            {"success", true},
            {"dsaStructure", "General Tree (N-ary Tree)"},
            {"description", "Hospital Department Hierarchy"},
            {"treeHeight", DepartmentService::getTree().getHeight()},
            {"hierarchy", hierarchy}
        };
        res.set_content(responseObj.dump(4), "application/json");
    });

    // Also support alias /api/departments/tree
    server.Get("/api/departments/tree", [](const httplib::Request& req, httplib::Response& res)
    {
        json hierarchy = DepartmentService::getHierarchyJson();
        res.set_content(hierarchy.dump(4), "application/json");
    });

    // GET /api/departments/stats - Operational metrics and department tree statistics
    server.Get("/api/departments/stats", [](const httplib::Request&, httplib::Response& res)
    {
        json stats = DepartmentService::getDepartmentStats();
        res.set_content(stats.dump(4), "application/json");
    });

    // GET /api/departments/search - Recursive tree search for departments
    server.Get("/api/departments/search", [](const httplib::Request& req, httplib::Response& res)
    {
        string query = "";
        if (req.has_param("q"))
        {
            query = req.get_param_value("q");
        }
        else if (req.has_param("search"))
        {
            query = req.get_param_value("search");
        }

        vector<Department> matches = DepartmentService::searchDepartments(query);

        json resultsArray = json::array();
        for (const auto& dept : matches)
        {
            json obj = departmentToJson(dept);
            obj["breadcrumbPath"] = DepartmentService::getDepartmentPath(dept.id);
            resultsArray.push_back(obj);
        }

        json responseObj = {
            {"success", true},
            {"dsaOperation", "Recursive Tree Search"},
            {"query", query},
            {"count", matches.size()},
            {"results", resultsArray}
        };

        res.set_content(responseObj.dump(4), "application/json");
    });

    // GET /api/departments - List all departments (flat list with metadata)
    server.Get("/api/departments", [](const httplib::Request& req, httplib::Response& res)
    {
        const auto& departments = DepartmentService::getAllDepartments();

        json result = json::array();
        for (const auto& dept : departments)
        {
            json obj = departmentToJson(dept);
            obj["breadcrumbPath"] = DepartmentService::getDepartmentPath(dept.id);
            result.push_back(obj);
        }

        res.set_content(result.dump(4), "application/json");
    });

    // GET /api/departments/:id/subdepartments - Get child departments using N-ary Tree recursion
    server.Get(R"(/api/departments/(\d+)/subdepartments)",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        Department* dept = DepartmentService::findDepartment(id);

        if (!dept)
        {
            setError(res, 404, "Department not found.");
            return;
        }

        bool recursive = true;
        if (req.has_param("recursive"))
        {
            string val = req.get_param_value("recursive");
            recursive = (val == "true" || val == "1" || val == "yes");
        }

        vector<Department> subdepts = DepartmentService::getSubdepartments(id, recursive);

        json subdeptsArray = json::array();
        for (const auto& sub : subdepts)
        {
            json obj = departmentToJson(sub);
            obj["breadcrumbPath"] = DepartmentService::getDepartmentPath(sub.id);
            subdeptsArray.push_back(obj);
        }

        json responseObj = {
            {"success", true},
            {"parentDepartmentId", id},
            {"parentDepartmentName", dept->name},
            {"recursive", recursive},
            {"count", subdepts.size()},
            {"subdepartments", subdeptsArray}
        };

        res.set_content(responseObj.dump(4), "application/json");
    });

    // GET /api/departments/:id/doctors - Doctors in department and its subtree
    server.Get(R"(/api/departments/(\d+)/doctors)",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        Department* dept = DepartmentService::findDepartment(id);

        if (!dept)
        {
            setError(res, 404, "Department not found.");
            return;
        }

        bool includeSubdepartments = false;
        if (req.has_param("includeSubdepartments"))
        {
            string val = req.get_param_value("includeSubdepartments");
            includeSubdepartments = (val == "true" || val == "1" || val == "yes");
        }

        vector<Doctor> doctors = DoctorService::filterDoctors(
            id, "", "", "", includeSubdepartments
        );

        json doctorsArray = json::array();
        for (const auto& doc : doctors)
        {
            doctorsArray.push_back({
                {"id", doc.id},
                {"name", doc.name},
                {"specialty", doc.specialty},
                {"departmentId", doc.departmentId},
                {"roomNumber", doc.roomNumber},
                {"availability", doc.availability},
                {"consultationFee", doc.consultationFee}
            });
        }

        json responseObj = {
            {"success", true},
            {"departmentId", id},
            {"departmentName", dept->name},
            {"includeSubdepartments", includeSubdepartments},
            {"count", doctors.size()},
            {"doctors", doctorsArray}
        };

        res.set_content(responseObj.dump(4), "application/json");
    });

    // GET /api/departments/:id - Get single department details with breadcrumbs & immediate children
    server.Get(R"(/api/departments/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        Department* dept = DepartmentService::findDepartment(id);

        if (!dept)
        {
            setError(res, 404, "Department not found.");
            return;
        }

        json result = departmentToJson(*dept);
        result["breadcrumbPath"] = DepartmentService::getDepartmentPath(id);

        auto immediateChildren = DepartmentService::getSubdepartments(id, false);
        json childrenArray = json::array();
        for (const auto& child : immediateChildren)
        {
            childrenArray.push_back({
                {"id", child.id},
                {"name", child.name},
                {"location", child.location}
            });
        }
        result["immediateChildDepartments"] = childrenArray;

        res.set_content(result.dump(4), "application/json");
    });

    // POST /api/departments - Add a new department
    server.Post("/api/departments", [](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            json body = json::parse(req.body);

            Department dept;
            dept.id = body.value("id", 0);
            dept.name = body.at("name").get<string>();
            dept.parentDepartmentId = body.value("parentDepartmentId", 0);
            dept.description = body.value("description", "");
            dept.location = body.value("location", "");
            dept.headDoctor = body.value("headDoctor", "");
            dept.status = body.value("status", "Active");

            if (dept.name.empty())
            {
                setError(res, 400, "Department name cannot be empty.");
                return;
            }

            if (!DepartmentService::addDepartment(dept))
            {
                setError(res, 409, "Failed to add department. Duplicate ID or parent department not found.");
                return;
            }

            Department* created = DepartmentService::findDepartment(dept.id);
            res.status = 201;
            res.set_content(
                json({
                    {"success", true},
                    {"message", "Department created successfully."},
                    {"department", created ? departmentToJson(*created) : departmentToJson(dept)}
                }).dump(4),
                "application/json"
            );
        }
        catch (const exception& e)
        {
            setError(res, 400, string("Invalid JSON payload: ") + e.what());
        }
    });

    // PUT /api/departments/:id - Update existing department
    server.Put(R"(/api/departments/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);
        try
        {
            json body = json::parse(req.body);

            Department updated;
            updated.id = id;
            updated.name = body.at("name").get<string>();
            updated.parentDepartmentId = body.value("parentDepartmentId", 0);
            updated.description = body.value("description", "");
            updated.location = body.value("location", "");
            updated.headDoctor = body.value("headDoctor", "");
            updated.status = body.value("status", "");

            string errorMsg;
            if (!DepartmentService::updateDepartment(id, updated, errorMsg))
            {
                setError(res, errorMsg == "Department not found." ? 404 : 400, errorMsg);
                return;
            }

            Department* dept = DepartmentService::findDepartment(id);
            res.set_content(
                json({
                    {"success", true},
                    {"message", "Department updated successfully."},
                    {"department", departmentToJson(*dept)}
                }).dump(4),
                "application/json"
            );
        }
        catch (const exception& e)
        {
            setError(res, 400, string("Invalid JSON payload: ") + e.what());
        }
    });

    // DELETE /api/departments/:id - Remove department from hierarchy
    server.Delete(R"(/api/departments/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        int id = stoi(req.matches[1]);

        string errorMsg;
        if (!DepartmentService::deleteDepartment(id, errorMsg))
        {
            setError(res, errorMsg == "Department not found." ? 404 : 400, errorMsg);
            return;
        }

        res.set_content(
            json({
                {"success", true},
                {"message", "Department removed successfully. Child departments have been reparented."}
            }).dump(4),
            "application/json"
        );
    });
}
