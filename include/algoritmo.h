#ifndef ALGORITMO_H
#define ALGORITMO_H

#include <QString>
#include <vector>
#include "proceso.h"
#include "tipos.h"

class GanttWindow;

// ----- Algoritmos de Scheduling (Simulación A) -----
std::vector<Proceso> fifo(const std::vector<Proceso>& procesos, GanttWindow* gantt = nullptr );
std::vector<Proceso> roundRobin(
    const std::vector<Proceso>& procesos,
    int quantum,
    std::vector<BloqueGantt>& bloques,
    GanttWindow* gantt = nullptr
);
std::vector<Proceso> shortestJobFirst(const std::vector<Proceso>& procesos, GanttWindow* gantt = nullptr);
std::vector<Proceso> priorityScheduling(const std::vector<Proceso>& procesos, GanttWindow* gantt = nullptr);
std::vector<Proceso> shortestRemainingTime(
    const std::vector<Proceso>& procesosOriginal,
    std::vector<BloqueGantt>& bloques,
    GanttWindow* gantt = nullptr
);
double calcularTiempoEsperaPromedio(
    const std::vector<Proceso>& procesosOriginal,
    const std::vector<Proceso>& ejecucion
);

void delay(int milisegundos);

#endif