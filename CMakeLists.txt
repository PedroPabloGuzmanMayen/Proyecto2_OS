cmake_minimum_required(VERSION 3.10)
project(SimuladorSO)

set(CMAKE_CXX_STANDARD 17)

include_directories(include gui)

file(GLOB SRC_FILES src/*.cpp gui/*.cpp)

find_package(FLTK REQUIRED)

add_executable(simulador ${SRC_FILES})
target_link_libraries(simulador fltk)