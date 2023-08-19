#include "condvar.h"
#include "../fiber/fiber_impl.h"
#include "../fiber/fiber_manager.h"


extern FiberManager fiberManager;
extern thread_local std::shared_ptr<FiberImpl> current_fiber;

void CondVar::wait() {
    {
        std::lock_guard<std::mutex> lg(lock);
        waiters.push(current_fiber);
    }
    current_fiber->ready = false;
    current_fiber->suspend();
}

void CondVar::notify_one() {
    std::shared_ptr<FiberImpl> fiber_to_wake;
    {
        std::lock_guard<std::mutex> lg(lock);
        if (!waiters.empty()) {
            fiber_to_wake = waiters.front();
            waiters.pop();
        }
    }
    if (fiber_to_wake) {
        fiber_to_wake->ready = true;
        fiberManager.registerFiber(fiber_to_wake);
    }
}

void CondVar::notify_all() {
    std::queue<std::shared_ptr<FiberImpl>> old_queue;
    {
        std::lock_guard<std::mutex> lg(lock);
        old_queue = waiters;
        waiters = std::queue<std::shared_ptr<FiberImpl>>();
    }
    while (!old_queue.empty()) {
        auto fiber = old_queue.front();
        old_queue.pop();
        fiber->ready = true;
        fiberManager.registerFiber(fiber);
    }
}

CondVar::CondVar() = default;
