#pragma once

#include "fiber_impl.h"
#include "fiber_manager.h"

using std::shared_ptr;

extern FiberManager fiberManager;

class Fiber {
public:
    template<typename Callable, typename... Args>
    Fiber(const Callable& function, const Args&... args) {
        fiber_ptr = std::make_shared<FiberImpl>(([args..., function] () {
            function(args...);
        }));
        fiber_ptr->start();
    }

    ~Fiber() = default;

    void join() {
        fiber_ptr->join();
    }

    void detach() {
        fiberManager.work();
    }

    bool isFinished() {
        return fiber_ptr->isFinished();
    }

    bool isReady() {
        return fiber_ptr->ready;
    }

private:
     shared_ptr<FiberImpl> fiber_ptr;
};