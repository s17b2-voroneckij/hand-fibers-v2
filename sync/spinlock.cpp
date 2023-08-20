#include "spinlock.h"
#include <sched.h>

void Spinlock::lock() {
    while (locked.exchange(true)) {
    }
}

void Spinlock::unlock() {
    locked.exchange(false);
}
