cmake_minimum_required(VERSION 3.10)
project(SimuladorSO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

# MOC manual de ganttwindow.h (necesita QObject por el timer/slots)
qt5_wrap_cpp(MOC_SOURCES ${CMAKE_SOURCE_DIR}/include/ganttwindow.h)

add_executable(simulador
    # Lógica principal
    src/main.cpp
    src/algoritmo.cpp
    src/proceso.cpp
    src/priorityQueue.cpp
    src/ganttwindow.cpp
    src/synchronizer.cpp
    

    # Interfaz gráfica
    gui/gui.cpp
    gui/mainmenu.cpp
    gui/estadisticas.cpp

    ${MOC_SOURCES}
)

target_link_libraries(simulador Qt5::Widgets)

add_executable(tests
    test/tests.cpp

    src/algoritmo.cpp
    src/proceso.cpp
    src/priorityQueue.cpp
    src/ganttwindow.cpp
    src/synchronizer.cpp

    ${MOC_SOURCES}
)

target_link_libraries(tests Qt5::Widgets)