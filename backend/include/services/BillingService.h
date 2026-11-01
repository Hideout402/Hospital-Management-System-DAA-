#ifndef BILLINGSERVICE_H
#define BILLINGSERVICE_H

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include "../data/DataStore.h"

class BillingService
{
public:
    static vector<Bill>& getAllBills()
    {
        return DataStore::getBills();
    }

    static Bill* findBill(int id)
    {
        for (auto& bill : DataStore::getBills())
        {
            if (bill.id == id)
                return &bill;
        }

        return nullptr;
    }

    static double calculateLineItemAmount(const BillLineItem& item)
    {
        return item.quantity * item.unitPrice;
    }

    static string determinePaymentStatus(double total, double paidAmount)
    {
        if (total <= 0.0)
            return "Paid";

        if (paidAmount <= 0.0)
            return "Unpaid";

        if (paidAmount >= total)
            return "Paid";

        return "Partially Paid";
    }

    static void calculateBillTotals(Bill& bill)
    {
        bill.subtotal = 0.0;

        for (BillLineItem& item : bill.lineItems)
        {
            if (item.quantity <= 0)
                throw invalid_argument("Line item quantity must be greater than zero.");

            if (item.unitPrice < 0.0)
                throw invalid_argument("Line item unit price cannot be negative.");

            item.amount = calculateLineItemAmount(item);
            bill.subtotal += item.amount;
        }

        if (bill.discount < 0.0)
            throw invalid_argument("Discount cannot be negative.");

        if (bill.paidAmount < 0.0)
            throw invalid_argument("Paid amount cannot be negative.");

        bill.total = max(0.0, bill.subtotal - bill.discount);
        bill.balance = max(0.0, bill.total - bill.paidAmount);
        bill.status = determinePaymentStatus(bill.total, bill.paidAmount);
    }

    static bool validatePatientReference(int patientId, string& error)
    {
        auto& patients = DataStore::getPatients();

        for (const auto& patient : patients)
        {
            if (patient.id == patientId)
                return true;
        }

        error = "Patient not found.";
        return false;
    }

    static bool addBill(Bill bill, string& error)
    {
        error.clear();

        if (bill.id <= 0)
        {
            error = "Bill ID must be greater than zero.";
            return false;
        }

        if (findBill(bill.id) != nullptr)
        {
            error = "Bill ID already exists.";
            return false;
        }

        if (bill.patientId <= 0 || !validatePatientReference(bill.patientId, error))
            return false;

        if (bill.patientName.empty())
        {
            for (const auto& patient : DataStore::getPatients())
            {
                if (patient.id == bill.patientId)
                {
                    bill.patientName = patient.name;
                    break;
                }
            }
        }

        if (bill.lineItems.empty())
        {
            error = "At least one line item is required.";
            return false;
        }

        try
        {
            calculateBillTotals(bill);
        }
        catch (const invalid_argument& ex)
        {
            error = ex.what();
            return false;
        }

        if (bill.paidAmount > bill.total)
        {
            error = "Paid amount cannot exceed the total bill amount.";
            return false;
        }

        DataStore::getBills().push_back(bill);
        return true;
    }

    static bool updateBill(int billId, Bill updatedBill, string& error)
    {
        error.clear();

        Bill* existingBill = findBill(billId);
        if (existingBill == nullptr)
        {
            error = "Bill not found.";
            return false;
        }

        if (updatedBill.patientId <= 0 || !validatePatientReference(updatedBill.patientId, error))
            return false;

        if (updatedBill.lineItems.empty())
        {
            error = "At least one line item is required.";
            return false;
        }

        if (updatedBill.patientName.empty())
        {
            for (const auto& patient : DataStore::getPatients())
            {
                if (patient.id == updatedBill.patientId)
                {
                    updatedBill.patientName = patient.name;
                    break;
                }
            }
        }

        try
        {
            calculateBillTotals(updatedBill);
        }
        catch (const invalid_argument& ex)
        {
            error = ex.what();
            return false;
        }

        if (updatedBill.paidAmount > updatedBill.total)
        {
            error = "Paid amount cannot exceed the total bill amount.";
            return false;
        }

        *existingBill = updatedBill;
        existingBill->id = billId;
        existingBill->status = determinePaymentStatus(existingBill->total, existingBill->paidAmount);
        existingBill->balance = max(0.0, existingBill->total - existingBill->paidAmount);
        return true;
    }

    static bool deleteBill(int id)
    {
        vector<Bill>& bills = DataStore::getBills();

        for (size_t i = 0; i < bills.size(); ++i)
        {
            if (bills[i].id == id)
            {
                bills.erase(bills.begin() + static_cast<int>(i));
                return true;
            }
        }

        return false;
    }

    static vector<Bill> searchBillsByPatientId(int patientId)
    {
        vector<Bill> results;

        for (const auto& bill : DataStore::getBills())
        {
            if (bill.patientId == patientId)
                results.push_back(bill);
        }

        return results;
    }

    static bool recordPayment(int billId, double amount, string& error)
    {
        error.clear();

        Bill* bill = findBill(billId);
        if (bill == nullptr)
        {
            error = "Bill not found.";
            return false;
        }

        if (amount < 0.0)
        {
            error = "Payment amount cannot be negative.";
            return false;
        }

        double outstanding = max(0.0, bill->total - bill->paidAmount);
        if (amount > outstanding)
        {
            error = "Payment exceeds the outstanding balance.";
            return false;
        }

        bill->paidAmount += amount;
        bill->balance = max(0.0, bill->total - bill->paidAmount);
        bill->status = determinePaymentStatus(bill->total, bill->paidAmount);
        return true;
    }

    static string getStatusForBill(const Bill& bill)
    {
        return determinePaymentStatus(bill.total, bill.paidAmount);
    }
};

#endif
