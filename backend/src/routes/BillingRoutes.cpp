#include "../../include/Routes.h"
#include "../../include/services/BillingService.h"
#include "../../include/services/PatientService.h"
#include "../../include/libraries/json.hpp"

using json = nlohmann::json;
using namespace std;

namespace {

json billToJson(const Bill& bill)
{
    json lineItems = json::array();

    for (const auto& item : bill.lineItems)
    {
        lineItems.push_back({
            {"description", item.description},
            {"quantity", item.quantity},
            {"unitPrice", item.unitPrice},
            {"amount", item.amount}
        });
    }

    return {
        {"id", bill.id},
        {"patientId", bill.patientId},
        {"patientName", bill.patientName},
        {"lineItems", lineItems},
        {"subtotal", bill.subtotal},
        {"discount", bill.discount},
        {"total", bill.total},
        {"paidAmount", bill.paidAmount},
        {"balance", bill.balance},
        {"status", bill.status}
    };
}

Bill billFromJson(const json& body, bool includeId = false)
{
    Bill bill{};

    if (includeId)
    {
        bill.id = body.value("id", 0);
    }

    bill.patientId = body.value("patientId", 0);
    bill.patientName = body.value("patientName", "");
    bill.discount = body.value("discount", 0.0);
    bill.paidAmount = body.value("paidAmount", 0.0);

    if (bill.patientId <= 0)
        throw invalid_argument("Patient ID is required.");

    if (body.contains("lineItems") && body["lineItems"].is_array())
    {
        for (const auto& itemJson : body["lineItems"])
        {
            BillLineItem item{};
            item.description = itemJson.value("description", "");
            item.quantity = itemJson.value("quantity", 0);
            item.unitPrice = itemJson.value("unitPrice", 0.0);

            if (item.description.empty())
                throw invalid_argument("Each line item requires a description.");

            item.amount = static_cast<double>(item.quantity) * item.unitPrice;
            bill.lineItems.push_back(item);
        }
    }

    if (bill.lineItems.empty())
        throw invalid_argument("At least one line item is required.");

    if (bill.patientName.empty())
    {
        Patient* patient = PatientService::findPatient(bill.patientId);
        if (patient != nullptr)
            bill.patientName = patient->name;
    }

    return bill;
}

} // namespace

void registerBillingRoutes(httplib::Server& server)
{
    server.Get("/api/bills", [](const httplib::Request&, httplib::Response& res)
    {
        json result = json::array();

        for (const auto& bill : BillingService::getAllBills())
        {
            result.push_back(billToJson(bill));
        }

        res.set_content(result.dump(4), "application/json");
    });

    server.Get(R"(/api/bills/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            int id = stoi(req.matches[1]);
            Bill* bill = BillingService::findBill(id);

            if (bill == nullptr)
            {
                res.status = 404;
                res.set_content(
                    R"({"success":false,"message":"Bill not found"})",
                    "application/json"
                );
                return;
            }

            res.set_content(billToJson(*bill).dump(4), "application/json");
        }
        catch (const exception&)
        {
            res.status = 400;
            res.set_content(
                R"({"success":false,"message":"Invalid bill ID"})",
                "application/json"
            );
        }
    });

    server.Post("/api/bills",
        [](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            json body = json::parse(req.body);
            Bill bill = billFromJson(body);
            string error;

            if (!BillingService::validatePatientReference(bill.patientId, error))
            {
                res.status = 400;
                res.set_content(json({{"success", false}, {"message", error}}).dump(4), "application/json");
                return;
            }

            if (!BillingService::addBill(bill, error))
            {
                res.status = 400;
                res.set_content(json({{"success", false}, {"message", error}}).dump(4), "application/json");
                return;
            }

            res.status = 201;
            res.set_content(json({{"success", true}, {"message", "Bill created successfully"}, {"bill", billToJson(bill)}}).dump(4), "application/json");
        }
        catch (const exception& ex)
        {
            res.status = 400;
            res.set_content(json({{"success", false}, {"message", ex.what()}}).dump(4), "application/json");
        }
    });

    server.Put(R"(/api/bills/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            int id = stoi(req.matches[1]);
            json body = json::parse(req.body);
            Bill bill = billFromJson(body);
            string error;

            if (!BillingService::updateBill(id, bill, error))
            {
                res.status = (error == "Bill not found.") ? 404 : 400;
                res.set_content(json({{"success", false}, {"message", error}}).dump(4), "application/json");
                return;
            }

            Bill* savedBill = BillingService::findBill(id);
            res.set_content(json({{"success", true}, {"message", "Bill updated successfully"}, {"bill", billToJson(*savedBill)}}).dump(4), "application/json");
        }
        catch (const exception& ex)
        {
            res.status = 400;
            res.set_content(json({{"success", false}, {"message", ex.what()}}).dump(4), "application/json");
        }
    });

    server.Delete(R"(/api/bills/(\d+))",
        [](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            int id = stoi(req.matches[1]);

            if (!BillingService::deleteBill(id))
            {
                res.status = 404;
                res.set_content(
                    R"({"success":false,"message":"Bill not found"})",
                    "application/json"
                );
                return;
            }

            res.set_content(
                R"({"success":true,"message":"Bill deleted successfully"})",
                "application/json"
            );
        }
        catch (const exception&)
        {
            res.status = 400;
            res.set_content(
                R"({"success":false,"message":"Invalid bill ID"})",
                "application/json"
            );
        }
    });

    server.Patch(R"(/api/bills/(\d+)/payment)",
        [](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            int id = stoi(req.matches[1]);
            json body = json::parse(req.body);
            double amount = body.value("amount", 0.0);
            string error;

            if (amount <= 0.0)
            {
                res.status = 400;
                res.set_content(json({{"success", false}, {"message", "Payment amount must be greater than zero."}}).dump(4), "application/json");
                return;
            }

            if (!BillingService::recordPayment(id, amount, error))
            {
                res.status = (error == "Bill not found.") ? 404 : 400;
                res.set_content(json({{"success", false}, {"message", error}}).dump(4), "application/json");
                return;
            }

            Bill* updatedBill = BillingService::findBill(id);
            res.set_content(json({{"success", true}, {"message", "Payment recorded successfully"}, {"bill", billToJson(*updatedBill)}}).dump(4), "application/json");
        }
        catch (const exception& ex)
        {
            res.status = 400;
            res.set_content(json({{"success", false}, {"message", ex.what()}}).dump(4), "application/json");
        }
    });
}
