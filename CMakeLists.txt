cmake_minimum_required(VERSION 3.24)
project(hand-fibers)

set(CMAKE_CXX_STANDARD 20)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")

find_package( Boost REQUIRED COMPONENTS context )

add_executable(tests main.cpp
        fiber/fiber_impl.cpp
        fiber/fiber_manager.cpp
        sync/mutex.cpp
        sync/condvar.cpp)

add_executable(server server.cpp
        fiber/fiber_impl.cpp
        fiber/fiber_manager.cpp
        sync/mutex.cpp
        sync/condvar.cpp
        io/waiter.cpp
)

target_link_libraries(tests PRIVATE Boost::boost Boost::context)
target_link_libraries(server PRIVATE Boost::boost Boost::context)