#ifndef EAPPOINTMENTCIRCULARQUEUE_H
#define EAPPOINTMENTCIRCULARQUEUE_H

#include <stdexcept>
#include <vector>

class EAppointmentCircularQueue
{
private:
    int* appointmentIds;
    int capacity;
    int frontIndex;
    int count;

public:
    explicit EAppointmentCircularQueue(int maxSize = 10)
        : appointmentIds(nullptr),
          capacity(maxSize),
          frontIndex(0),
          count(0)
    {
        if (maxSize <= 0)
            throw std::invalid_argument("Circular queue capacity must be positive.");
        appointmentIds = new int[maxSize];
    }

    EAppointmentCircularQueue(const EAppointmentCircularQueue&) = delete;
    EAppointmentCircularQueue& operator=(const EAppointmentCircularQueue&) = delete;

    ~EAppointmentCircularQueue()
    {
        delete[] appointmentIds;
    }

    bool enqueue(int appointmentId)
    {
        if (isFull())
            return false;
        const int rearIndex = (frontIndex + count) % capacity;
        appointmentIds[rearIndex] = appointmentId;
        ++count;
        return true;
    }

    int dequeue()
    {
        if (isEmpty())
            return -1;
        const int appointmentId = appointmentIds[frontIndex];
        frontIndex = (frontIndex + 1) % capacity;
        --count;
        return appointmentId;
    }

    int front() const
    {
        return isEmpty() ? -1 : appointmentIds[frontIndex];
    }

    bool rotate()
    {
        if (count < 2)
            return false;
        const int appointmentId = dequeue();
        return enqueue(appointmentId);
    }

    bool contains(int appointmentId) const
    {
        for (int i = 0; i < count; ++i)
        {
            const int index = (frontIndex + i) % capacity;
            if (appointmentIds[index] == appointmentId)
                return true;
        }
        return false;
    }

    bool remove(int appointmentId)
    {
        int offset = 0;
        while (offset < count &&
            appointmentIds[(frontIndex + offset) % capacity] != appointmentId)
            ++offset;
        if (offset == count)
            return false;

        for (int i = offset; i < count - 1; ++i)
        {
            const int current = (frontIndex + i) % capacity;
            const int next = (frontIndex + i + 1) % capacity;
            appointmentIds[current] = appointmentIds[next];
        }
        --count;
        return true;
    }

    bool isEmpty() const
    {
        return count == 0;
    }

    bool isFull() const
    {
        return count == capacity;
    }

    int size() const
    {
        return count;
    }

    int maxSize() const
    {
        return capacity;
    }

    std::vector<int> toVector() const
    {
        std::vector<int> result;
        result.reserve(static_cast<size_t>(count));
        for (int i = 0; i < count; ++i)
            result.push_back(appointmentIds[(frontIndex + i) % capacity]);
        return result;
    }
};

#endif
