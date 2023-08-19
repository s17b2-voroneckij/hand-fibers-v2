#include "fiber_impl.h"
#include "fiber_manager.h"

using std::cerr;

extern FiberManager fiberManager;

extern thread_local FiberImpl* current_fiber;

FiberImpl::FiberImpl(const std::function<void()> &func) {
    this->func = func;
}

void FiberImpl::join() {
    while (!finished) {
        finish_cv.wait();
    }
}

bool FiberImpl::isFinished() {
    return finished;
}

void FiberImpl::start() {
    ready = true;
    fiberManager.registerFiber(shared_from_this());
}

void FiberImpl::continue_executing() {
    if (!launched) {
        launched = true;
        this_context = callcc([&](auto sink) {
            previous_context = std::move(sink);
            func();
            finished = true;
            ready = false;
            finish_cv.notify_all();
            return std::move(previous_context);
        });
    } else {
        this_context = this_context.resume();
    }
}

void FiberImpl::suspend() {
    previous_context = previous_context.resume();
}

void sched_execution() {
    current_fiber->suspend();
}

FiberImpl::~FiberImpl() {
}
