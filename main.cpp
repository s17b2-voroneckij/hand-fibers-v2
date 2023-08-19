#include <iostream>
#include "fiber/fiber.hpp"
#include "sync/mutex.h"

using std::cerr;
using std::endl;

int printed = 0;

void print_number(int a) {
    cerr << a << endl;
    printed++;
    if (printed == 2) {
        requestShutDown();
    }
}

void test_func() {
    static int global_id = 0;
    int this_id = global_id++;
    cerr << "test_func entering " << this_id << endl;
    sched_execution();
    cerr << "test_func leaving " << this_id << endl;
}

void ping_pong(int id) {
    static bool global_cnt = false;
    static CondVar cv;
    bool allowed = global_cnt;
    global_cnt = !global_cnt;
    for (int i = 0; i < 1; i++) {
        if (allowed) {
            cerr << id << " notify" << endl;
            cv.notify_one();
            allowed = false;
        } else {
            cerr << id << " waiting" << endl;
            cv.wait();
            cerr << id << " waited" << endl;
            allowed = true;
        }
    }
    cerr << id << " ping_pong leaving\n";
}

Mutex m;
int inside = 0;

void test_mutex() {
    static int counter = 0;
    int id = counter++;
    for (int i = 0; i < 200; i++) {
        cerr << id << " going to lock" << endl;
        m.lock();
        inside++;
        cerr << id << " inside: " << inside << endl;
        sched_execution();
        inside--;
        m.unlock();
        sched_execution();
    }
    cerr << id << " leaving\n";
}

void tests() {
    //Fiber f3(print_number, 8), f4(print_number, 10);
    //Fiber f1(test_func), f2(test_func), f3(test_func);
    //startFiberManagerThread();
    //Fiber f1(ping_pong, 1), f2(ping_pong, 2);
    //startFiberManager();
    startFiberManagerThread();
    int number_of_threads = 200;
    std::vector<Fiber> threads;
    threads.reserve(number_of_threads);
    for (int i = 0; i < number_of_threads; i++) {
        threads.emplace_back(test_mutex);
    }
    startFiberManager();
}

int main() {
    tests();
}
