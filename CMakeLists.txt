cmake_minimum_required(VERSION 2.8)
project(HivekVM)

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

set(SOURCES
    src/HivekVM/HivekVM.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
