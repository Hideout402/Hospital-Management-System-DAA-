#include <iostream>
#include "../include/services/EmergencyService.h"
using namespace std;

int main()
{
    cout << "=== Test 1: heap order ===" << endl;
    EmergencyPriorityQueue pq;
    pq.insert({10, 1001, 3, 1, "Waiting", "A"});
    pq.insert({11, 1002, 1, 2, "Waiting", "B"});
    pq.insert({12, 1003, 1, 3, "Waiting", "C"});
    pq.insert({13, 1004, 5, 4, "Waiting", "D"});
    pq.insert({14, 1005, 2, 5, "Waiting", "E"});

    cout << "Expected: 11 12 14 10 13" << endl;
    cout << "Actual:   ";
    EmergencyCase c;
    while (pq.extractMin(c))
        cout << c.id << " ";
    cout << endl;
    cout << "Extract from empty heap: " << (pq.extractMin(c) ? "true" : "false") << " (expected false)" << endl;

    cout << endl << "=== Test 2: priority order from MockData ===" << endl;
    for (const auto& x : EmergencyService::getPriorityOrder())
        cout << "case " << x.id << " triage=" << x.triageLevel << " status=" << x.status << endl;

    cout << endl << "=== Test 3: treat next until none left ===" << endl;
    EmergencyCase treated;
    while (EmergencyService::treatNext(treated))
        cout << "Treated case " << treated.id << " (triage " << treated.triageLevel << ")" << endl;
    cout << "No more waiting cases." << endl;

    return 0;
}