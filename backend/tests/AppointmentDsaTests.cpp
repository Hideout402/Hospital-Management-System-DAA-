#include <cassert>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "../include/dsa/EAppointmentCircularQueue.h"
#include "../include/dsa/EAppointmentQueue.h"
#include "../include/dsa/EAppointmentSearching.h"
#include "../include/dsa/EAppointmentStack.h"
#include "../include/dsa/Sorting.h"
#include "../include/dsa/EAppointmentSorting.h"

using namespace std;

struct AppointmentRecord
{
    int id;
    int startMinute;
};

struct AppointmentStartTimeLess
{
    bool operator()(const AppointmentRecord& left, const AppointmentRecord& right) const
    {
        return left.startMinute < right.startMinute;
    }
};

bool isSortedByStartTime(const vector<AppointmentRecord>& appointments)
{
    for (size_t i = 1; i < appointments.size(); ++i)
    {
        if (appointments[i].startMinute < appointments[i - 1].startMinute)
            return false;
    }
    return true;
}

void testAppointmentSorting()
{
    const vector<AppointmentRecord> schedule = {
        {104, 14 * 60 + 30},
        {101, 9 * 60},
        {103, 11 * 60 + 15},
        {102, 10 * 60},
        {105, 14 * 60 + 30}
    };

    vector<AppointmentRecord> bubbleSorted = schedule;
    bubbleSort(bubbleSorted, AppointmentStartTimeLess());
    assert(isSortedByStartTime(bubbleSorted));
    cout << "PASS: Bubble Sort orders appointment times" << endl;

    vector<AppointmentRecord> mergeSorted = schedule;
    mergeSort(mergeSorted, 0, static_cast<int>(mergeSorted.size()) - 1, AppointmentStartTimeLess());
    assert(isSortedByStartTime(mergeSorted));
    cout << "PASS: Merge Sort orders appointment times" << endl;

    vector<AppointmentRecord> quickSorted = schedule;
    quickSort(quickSorted, AppointmentStartTimeLess());
    assert(isSortedByStartTime(quickSorted));
    cout << "PASS: Quick Sort orders appointment times" << endl;

    vector<int> duplicateIds = {8, 3, 3, 1, 8, 2, 1};
    quickSort(duplicateIds);
    assert((duplicateIds == vector<int>{1, 1, 2, 3, 3, 8, 8}));

    vector<int> descendingIds = {1, 4, 2, 5, 3};
    quickSort(descendingIds, greater<int>());
    assert((descendingIds == vector<int>{5, 4, 3, 2, 1}));

    vector<int> emptySchedule;
    quickSort(emptySchedule);
    assert(emptySchedule.empty());

    vector<int> oneAppointment = {42};
    quickSort(oneAppointment);
    assert(oneAppointment[0] == 42);

    for (int size = 0; size <= 128; ++size)
    {
        vector<int> ids;
        for (int index = 0; index < size; ++index)
            ids.push_back((index * 37 + size * 13) % 31 - 15);

        quickSort(ids);
        assert(is_sorted(ids.begin(), ids.end()));
    }

    cout << "PASS: Quick Sort handles duplicates, custom order, and small ranges" << endl;

    vector<int> rangeIds = {9, 5, 4, 3, 8};
    quickSort(rangeIds, 1, 3);
    assert((rangeIds == vector<int>{9, 3, 4, 5, 8}));
    cout << "PASS: Quick Sort preserves values outside the selected range" << endl;

    vector<AppointmentRecord> idSorted = schedule;
    quickSort(idSorted, [](const AppointmentRecord& left, const AppointmentRecord& right) {
        return left.id < right.id;
    });
    const int foundIndex = appointmentBinarySearch(
        idSorted, 103, [](const AppointmentRecord& appointment) { return appointment.id; });
    assert(foundIndex >= 0 && idSorted[foundIndex].id == 103);
    assert(appointmentBinarySearch(
        idSorted, 999, [](const AppointmentRecord& appointment) { return appointment.id; }) == -1);
    cout << "PASS: Binary Search finds and rejects appointment IDs in ID-sorted records" << endl;
}

void testAppointmentQueuesAndStack()
{
    EAppointmentQueue checkInQueue;
    checkInQueue.enqueue(201);
    checkInQueue.enqueue(202);
    assert(checkInQueue.front() == 201);
    assert(checkInQueue.dequeue() == 201);
    assert(checkInQueue.front() == 202);
    assert(checkInQueue.dequeue() == 202);
    assert(checkInQueue.isEmpty());
    assert(checkInQueue.front() == -1);
    cout << "PASS: Normal queue processes checked-in appointment IDs FIFO" << endl;

    EAppointmentCircularQueue waitingRoomRotation(3);
    for (int id = 301; id <= 303; ++id)
        assert(waitingRoomRotation.enqueue(id));
    assert(waitingRoomRotation.isFull());
    assert(!waitingRoomRotation.enqueue(304));
    assert(waitingRoomRotation.front() == 301);
    assert(waitingRoomRotation.dequeue() == 301);
    assert(waitingRoomRotation.enqueue(304));
    assert((waitingRoomRotation.toVector() == vector<int>{302, 303, 304}));
    assert(waitingRoomRotation.rotate());
    assert((waitingRoomRotation.toVector() == vector<int>{303, 304, 302}));
    assert(waitingRoomRotation.dequeue() == 303);
    assert(waitingRoomRotation.dequeue() == 304);
    assert(waitingRoomRotation.dequeue() == 302);
    assert(waitingRoomRotation.isEmpty());
    assert(waitingRoomRotation.dequeue() == -1);
    cout << "PASS: Circular queue handles full, empty, rotation, and physical wrap-around" << endl;

    EAppointmentStack rescheduleHistory;
    rescheduleHistory.push({401, "Rescheduled", "Moved appointment.", "2026-01-01 09:00:00"});
    rescheduleHistory.push({402, "Cancelled", "Cancelled appointment.", "2026-01-01 09:01:00"});
    EAppointmentAction latest{};
    assert(rescheduleHistory.peek(latest));
    assert(latest.appointmentId == 402);
    assert(rescheduleHistory.pop(latest));
    assert(latest.appointmentId == 402);
    assert(rescheduleHistory.peek(latest));
    assert(latest.appointmentId == 401);
    assert(rescheduleHistory.pop(latest));
    assert(rescheduleHistory.isEmpty());
    cout << "PASS: Stack tracks appointment IDs in LIFO reschedule-history order" << endl;
}

int main()
{
    testAppointmentSorting();
    testAppointmentQueuesAndStack();
    cout << "All appointment DSA tests passed." << endl;
    return 0;
}
