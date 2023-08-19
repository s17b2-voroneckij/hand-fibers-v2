#include "fiber_manager.h"
#include "fiber.hpp"

#include <thread>

FiberManager fiberManager;

thread_local std::shared_ptr<FiberImpl> current_fiber;

void FiberManager::work() {
    while (!stop_requested) {
        shared_ptr<FiberImpl> next_fiber(nullptr);
        {
            std::lock_guard<std::mutex> lg(queue_lock);
            if (!ready_fibers.empty()) {
                next_fiber = ready_fibers.front();
                ready_fibers.pop();
                ready_fibers_set.erase(next_fiber);
                next_fiber->in_queue = false;
            }
        }
        if (next_fiber == nullptr) {
            sleep(1);
        } else {
            // execute this thread
            if (next_fiber->ready) {
                current_fiber = next_fiber;
                if (next_fiber->is_executing) {
                    printf("IS EXECUTING!!!!!");
                }
                next_fiber->is_executing = true;
                next_fiber->continue_executing();
                next_fiber->is_executing = false;
                current_fiber = nullptr;
            }
            if (next_fiber->ready && !next_fiber->in_queue.exchange(true)) {
                std::lock_guard<std::mutex> lg(queue_lock);
                if (ready_fibers_set.contains(next_fiber)) {
                    int fd = 0;
                }
                ready_fibers_set.insert(next_fiber);
                ready_fibers.push(next_fiber);
            }
        }
    }
}

void FiberManager::registerFiber(const shared_ptr<FiberImpl>& fiber_ptr) {
    if (!fiber_ptr->in_queue.exchange(true)) {
        std::lock_guard<std::mutex> lg(queue_lock);
        if (ready_fibers_set.contains(fiber_ptr)) {
            int fd = 0;
        }
        ready_fibers_set.insert(fiber_ptr);
        ready_fibers.push(fiber_ptr);
    }
}

FiberManager::FiberManager() = default;

void startFiberManagerThread() {
    std::thread thread([] {
        fiberManager.work();
    });
    thread.detach();
}

void requestShutDown() {
    fiberManager.stop_requested = true;
}

void startFiberManager() {
    fiberManager.work();
}
