#pragma once

#include <memory>
#include <queue>
#include <boost/lockfree/queue.hpp>
#include <set>
#include "fiber_impl.h"

void startFiberManager();

void requestShutDown();

void startFiberManagerThread();

class FiberManager {
public:
    FiberManager();

private:
    friend class Fiber;
    friend class FiberImpl;
    friend class CondVar;
    friend void sched_execution();
    friend void startFiberManagerThread();
    void work();
    void registerFiber(const std::shared_ptr<FiberImpl>& fiber_ptr);

    std::queue<std::shared_ptr<FiberImpl>> ready_fibers;
    std::set<std::shared_ptr<FiberImpl>> ready_fibers_set;
    std::mutex queue_lock;
    std::atomic<bool> stop_requested = false;

    friend void startFiberManager();

    friend void requestShutDown();
};