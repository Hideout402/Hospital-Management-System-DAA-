#ifndef EAPPOINTMENTDSASERVICE_H
#define EAPPOINTMENTDSASERVICE_H

#include <ctime>
#include <mutex>
#include <string>
#include <vector>

#include "../dsa/EAppointmentCircularQueue.h"
#include "../dsa/EAppointmentQueue.h"
#include "../dsa/EAppointmentStack.h"

class EAppointmentDsaService
{
private:
    static std::mutex& queueMutex()
    {
        static std::mutex mutex;
        return mutex;
    }

    static std::mutex& historyMutex()
    {
        static std::mutex mutex;
        return mutex;
    }

    static EAppointmentQueue& checkInQueue()
    {
        static EAppointmentQueue queue;
        return queue;
    }

    static EAppointmentCircularQueue& waitingRoomQueue()
    {
        static EAppointmentCircularQueue queue(10);
        return queue;
    }

    static EAppointmentStack& actionHistory()
    {
        static EAppointmentStack stack;
        return stack;
    }

public:
    static void recordAction(int appointmentId, const std::string& action, const std::string& details)
    {
        const std::time_t now = std::time(nullptr);
        std::tm localTime{};
#ifdef _WIN32
        localtime_s(&localTime, &now);
#else
        localtime_r(&now, &localTime);
#endif
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &localTime);
        std::lock_guard<std::mutex> lock(historyMutex());
        actionHistory().push({appointmentId, action, details, timestamp});
    }

    static void removeAppointmentFromQueues(int appointmentId)
    {
        std::lock_guard<std::mutex> lock(queueMutex());
        checkInQueue().remove(appointmentId);
        waitingRoomQueue().remove(appointmentId);
    }

    static std::vector<int> getCheckInQueue()
    {
        std::lock_guard<std::mutex> lock(queueMutex());
        return checkInQueue().toVector();
    }

    static bool enqueueCheckIn(int appointmentId)
    {
        std::lock_guard<std::mutex> lock(queueMutex());
        if (checkInQueue().contains(appointmentId))
            return false;
        checkInQueue().enqueue(appointmentId);
        return true;
    }

    static bool dequeueCheckIn(int& appointmentId)
    {
        std::lock_guard<std::mutex> lock(queueMutex());
        if (checkInQueue().isEmpty())
            return false;
        appointmentId = checkInQueue().dequeue();
        return true;
    }

    static std::vector<int> getWaitingRoomQueue()
    {
        std::lock_guard<std::mutex> lock(queueMutex());
        return waitingRoomQueue().toVector();
    }

    static int getWaitingRoomCapacity()
    {
        return waitingRoomQueue().maxSize();
    }

    static bool enqueueWaitingRoom(int appointmentId)
    {
        std::lock_guard<std::mutex> lock(queueMutex());
        if (waitingRoomQueue().contains(appointmentId))
            return false;
        return waitingRoomQueue().enqueue(appointmentId);
    }

    static bool dequeueWaitingRoom(int& appointmentId)
    {
        std::lock_guard<std::mutex> lock(queueMutex());
        if (waitingRoomQueue().isEmpty())
            return false;
        appointmentId = waitingRoomQueue().dequeue();
        return true;
    }

    static bool rotateWaitingRoom(int& appointmentId)
    {
        std::lock_guard<std::mutex> lock(queueMutex());
        if (waitingRoomQueue().isEmpty())
            return false;
        appointmentId = waitingRoomQueue().front();
        return waitingRoomQueue().rotate();
    }

    static std::vector<EAppointmentAction> getActionHistory()
    {
        std::lock_guard<std::mutex> lock(historyMutex());
        return actionHistory().toVector();
    }

    static bool popActionHistory(EAppointmentAction& action)
    {
        std::lock_guard<std::mutex> lock(historyMutex());
        return actionHistory().pop(action);
    }

    static bool peekActionHistory(EAppointmentAction& action)
    {
        std::lock_guard<std::mutex> lock(historyMutex());
        return actionHistory().peek(action);
    }
};

#endif
