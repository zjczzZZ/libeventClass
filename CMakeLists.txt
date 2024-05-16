cmake_minimum_required(VERSION 3.0)

project(libeventClass)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -levent -levent_pthreads -lpthread")

add_executable(main MultiServer.cpp test.cpp)

target_link_libraries(main libevent.so libevent_pthreads.so  libpthread.so)
