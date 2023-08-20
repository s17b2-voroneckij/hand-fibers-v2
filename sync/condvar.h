#pragma once

#include <queue>
#include <memory>
#include <mutex>
#include "spinlock.h"

class FiberImpl;

class CondVar {
public:
    CondVar();
    void wait();
    void notify_one();
    void notify_all();

private:
    std::queue<std::shared_ptr<FiberImpl>> waiters;
    Spinlock lock;
};