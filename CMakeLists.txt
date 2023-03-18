cmake_minimum_required(VERSION 3.5)
project(me_against_comcurrency)

include_directories(include)

add_library(CONCURRENT
        src/concurrent/mutex.cpp
        src/concurrent/static_thread_pool.cpp)

add_subdirectory(examples)
