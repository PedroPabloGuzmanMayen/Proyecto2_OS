cmake_minimum_required(VERSION 3.10)
project(SimuladorSO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

include_directories(include gui)

# MOC manual de ganttwindow.h
qt5_wrap_cpp(MOC_SOURCES include/ganttwindow.h)

add_executable(simulador
    src/main.cpp
    src/algoritmo.cpp
    src/proceso.cpp
    src/ganttwindow.cpp
    gui/gui.cpp
    ${MOC_SOURCES}
)
target_link_libraries(simulador Qt5::Widgets)

# Para la parte de sincronización (Simulación B)
add_executable(simulador_B
    src/main_sync.cpp
    src/synchronizer.cpp
    src/proceso.cpp
)
target_include_directories(simulador_B PRIVATE include)
target_link_libraries(simulador_B PRIVATE Qt5::Core)
