#ifndef ALGORITMO_H
#define ALGORITMO_H

#include "proceso.h"
#include <vector>

std::vector<Proceso> fifo(const std::vector<Proceso>& procesos);
std::vector<Proceso> roundRobin(const std::vector<Proceso>& procesos, int quantum);
double calcularTiempoEsperaPromedio(const std::vector<Proceso>& procesosOriginal, const std::vector<Proceso>& ejecucion);

#endif