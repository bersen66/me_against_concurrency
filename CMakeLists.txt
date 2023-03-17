cmake_minimum_required(VERSION 3.24)
project(me_against_comcurrency)

set(CMAKE_CXX_STANDARD 20)


include_directories(include)

add_library(
        CONCURRENT
        src/concurrent/mutex.cpp
)

add_executable(me_against_comcurrency src/main.cpp)
target_link_libraries(${PROJECT_NAME} CONCURRENT)
