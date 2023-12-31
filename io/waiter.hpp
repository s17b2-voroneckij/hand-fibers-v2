#include <unordered_map>
#include <atomic>

#include "../fiber/fiber_impl.h"
#include "../fiber/fiber_manager.h"
#include "../sync/condvar.h"
#include "../sync/mutex.h"

using std::unordered_map;

struct FdRequest {
    CondVar cv;
    int fd;
    short events;
};

class Waiter {
public:
    Waiter();

    static int wait(int fd, short events);

    static void stop();

    static void loop();

private:
    CondVar cv;
    Mutex map_mutex;
    unordered_map<int, FdRequest*> map;
    volatile std::atomic<bool> stopped = false;
};