#include "gui.h"
#include <FL/fl_message.H>
#include <iostream>
#include <vector>
#include "../src/proceso.h"

SimuladorGUI::SimuladorGUI() {
    ventana = new Fl_Window(400, 200, "Simulador de Sistemas Operativos");

    btnSimA = new Fl_Button(50, 70, 130, 40, "Simulación A");
    btnSimA->callback(onClickSimA, nullptr);

    btnSimB = new Fl_Button(220, 70, 130, 40, "Simulación B");
    btnSimB->callback(onClickSimB, nullptr);

    ventana->end();
}

void SimuladorGUI::show() {
    ventana->show();
}

void SimuladorGUI::onClickSimA(Fl_Widget*, void*) {
    std::vector<Proceso> procesos = cargarProcesosDesdeArchivo("data/procesos.txt");

    std::cout << "Se cargaron " << procesos.size() << " procesos desde el archivo." << std::endl;
    for (const auto& p : procesos) {
        std::cout << p.pid << ": BT=" << p.burstTime
                  << ", AT=" << p.arrivalTime
                  << ", Priority=" << p.priority << std::endl;
    }

    fl_message("Procesos cargados (ver terminal)");
}

void SimuladorGUI::onClickSimB(Fl_Widget*, void*) {
    fl_message("Simulación B: Mecanismos de Sincronización");
    // Aquí se llamará más adelante al módulo de sincronización
}