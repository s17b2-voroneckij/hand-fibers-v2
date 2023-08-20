#pragma once

#include <atomic>


class Spinlock {
public:
    void lock();

    void unlock();

private:
    std::atomic<bool> locked = false;
};


