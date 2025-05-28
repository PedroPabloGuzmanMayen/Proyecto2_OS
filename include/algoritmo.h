#ifndef ALGORITMO_H
#define ALGORITMO_H

#include "proceso.h"
#include <vector>

struct BloqueGantt {
    QString pid;
    int inicio;
    int duracion;
};

std::vector<Proceso> fifo(const std::vector<Proceso>& procesos);
std::vector<Proceso> roundRobin(const std::vector<Proceso>& procesos, int quantum, std::vector<BloqueGantt>& bloques);
double calcularTiempoEsperaPromedio(const std::vector<Proceso>& procesosOriginal, const std::vector<Proceso>& ejecucion);

#endif