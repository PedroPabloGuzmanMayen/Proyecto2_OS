cmake_minimum_required(VERSION 3.16)
project(Proyecto2_OS)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

include_directories(include gui)

file(GLOB SRC_FILES src/*.cpp gui/*.cpp)

add_executable(simulador ${SRC_FILES})
target_link_libraries(simulador Qt5::Widgets)