#ifndef ALGORITMO_H
#define ALGORITMO_H

#include <QString>
#include <vector>
#include "proceso.h"

// Estructura compartida para Bloques de Gantt (Simulación A)
struct BloqueGantt {
    QString pid;
    int inicio;
    int duracion;
};

// ----- Algoritmos de Scheduling (Simulación A) -----
std::vector<Proceso> fifo(const std::vector<Proceso>& procesos);
std::vector<Proceso> roundRobin(
    const std::vector<Proceso>& procesos,
    int quantum,
    std::vector<BloqueGantt>& bloques
);
std::vector<Proceso> shortestJobFirst(const std::vector<Proceso>& procesos);
std::vector<Proceso> priorityScheduling(const std::vector<Proceso>& procesos);
std::vector<Proceso> shortestRemainingTime(
    const std::vector<Proceso>& procesosOriginal,
    std::vector<BloqueGantt>& bloques
);
double calcularTiempoEsperaPromedio(
    const std::vector<Proceso>& procesosOriginal,
    const std::vector<Proceso>& ejecucion
);

#endif