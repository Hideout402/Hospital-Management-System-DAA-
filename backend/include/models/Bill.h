#ifndef BILL_H
#define BILL_H

#include <string>
#include <vector>
using namespace std;

struct BillLineItem {
    string description;
    int quantity;
    double unitPrice;
    double amount;
};

struct Bill {
    int id;
    int patientId;
    string patientName;
    vector<BillLineItem> lineItems;
    double subtotal;
    double discount;
    double total;
    double paidAmount;
    double balance;
    string status;
};

#endif
