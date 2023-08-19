#pragma once

#include <algorithm>
#include <atomic>
#include <iostream>
#include <memory>
#include <list>
#include <boost/context/continuation.hpp>

#include "../sync/condvar.h"
#include "fiber_manager.h"

using namespace boost::context;

class FiberManager;

void sched_execution();

class FiberImpl : public std::enable_shared_from_this<FiberImpl> {
public:
    ~FiberImpl();
    FiberImpl(const std::function<void()>& func);

private:
    void join();
    bool isFinished();
    void start();
    friend class Fiber;
    friend class FiberManager;
    friend class CondVar;
    friend void sched_execution();
    void continue_executing();
    void suspend();

    std::function<void()> func;
    continuation this_context;
    continuation previous_context;
    CondVar finish_cv;
    std::atomic<bool> launched = false;
    std::atomic<bool> finished = false;
    std::atomic<bool> ready = false;
    std::atomic<bool> is_executing = false;
    std::atomic<bool> in_queue = false;
};
