#include <iostream>
#include <vector>
#include <cstring>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <csignal>
#include "fiber/fiber_impl.h"
#include "io/waiter.hpp"
#include "fiber/fiber.hpp"

using std::cerr;
using std::endl;
using std::vector;

const int PORT = 8002;

unsigned long fibonacci(long n) {
    if (n <= 2) {
        return 1;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

template<typename Callable, typename... Args>
auto run_with_timing(Callable function, Args ...args) {
    auto start_time = clock();
    auto result = function(args...);
    //std::cout << "completed in " << ((double) clock() - start_time) / CLOCKS_PER_SEC << std::endl;
    return result;
}

void worker(int fd) {
    char buf[1024];
    while (true) {
        Waiter::wait(fd, POLLIN);
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n == 0) {
            printf("client finished, leaving\n");
            return;
        }
        if (n < 0) {
            printf("in worker error: %s\n", strerror(errno));
            return;
        }
        long request = strtol(buf, nullptr, 10);
        auto response = run_with_timing(fibonacci, request);
        auto response_string = "response: " + std::to_string(response) + "\n";
        strcpy(buf, response_string.c_str());
        n = strlen(buf);
        int wrote = 0;
        while (wrote < n) {
            Waiter::wait(fd, POLLOUT);
            ssize_t m = write(fd, buf + wrote, n - wrote);
            if (m < 0) {
                printf("in worker error: %s\n", strerror(errno));
                return;
            }
            wrote += m;
        }
    }
}

void signal_handler(int) {

}

void server() {
    std::cerr << "pid: " << getpid() << std::endl;
    //signal(SIGTERM, signal_handler);
    //signal(SIGINT, signal_handler);
    //signal(SIGABRT, signal_handler);
    std::cerr << "main entered" << endl;
    Fiber global_fiber([]() {
        int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_fd < 0) {
            printf("socket error: %s\n", strerror(errno));
            exit(0);
        }
        int ret = fcntl(socket_fd, F_SETFL, O_NONBLOCK);
        if (ret == -1) {
            printf("fcntl error: %s\n", strerror(errno));
            exit(0);
        }
        sockaddr_in sin{};
        sin.sin_family = AF_INET;
        sin.sin_port = htons(PORT);
        sin.sin_addr = in_addr{0};
        if (bind(socket_fd, reinterpret_cast<const sockaddr *>(&sin), sizeof(sin)) < 0) {
            printf("bind error: %s\n", strerror(errno));
            exit(0);
        }
        if (listen(socket_fd, 10) < 0) {
            printf("listen error: %s\n", strerror(errno));
            exit(0);
        }
        while (true) {
            Waiter::wait(socket_fd, POLLIN);
            int client_fd = accept4(socket_fd, NULL, NULL, SOCK_NONBLOCK);
            Fiber(worker, client_fd);
        }
        std::cerr << "main thread leaving\n";
    });
    for (int i = 0; i < 3; i++) {
        startFiberManagerThread();
    }
    startFiberManager();
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    server();
}
