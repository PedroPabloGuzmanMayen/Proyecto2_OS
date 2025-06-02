#include <iostream>
#include "synchronizer.h"

int main() {
    auto procesos = loadProcesos("data/procesos.txt");
    auto recursos = loadRecursos("data/recursos.txt");
    auto acciones = loadAcciones("data/acciones.txt");

    if (procesos.empty()) {
        std::cerr << "¡Error! No se cargaron procesos.\n";
        return 1;
    }
    if (recursos.empty()) {
        std::cerr << "¡Error! No se cargaron recursos.\n";
        return 1;
    }
    if (acciones.empty()) {
        std::cerr << "¡Error! No se cargaron acciones.\n";
        return 1;
    }

    auto timeline = simulateSync(acciones, recursos);

    std::cout << "Simulación B: Mutex/Semáforo\n"
              << "PID   Recurso  Start Dur Estado\n"
              << "-------------------------------\n";
    for (auto &b : timeline) {
        std::cout << b.pid << "    "
                  << b.recurso << "       "
                  << b.start   << "    "
                  << b.duration<< "   "
                  << (b.accessed ? "ACCESS" : "WAIT")
                  << "\n";
    }
    return 0;
}
