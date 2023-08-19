#pragma once

#include <queue>
#include <memory>
#include <mutex>

class FiberImpl;

class CondVar {
public:
    CondVar();
    void wait();
    void notify_one();
    void notify_all();

private:
    std::queue<std::shared_ptr<FiberImpl>> waiters;
    std::mutex lock;
};