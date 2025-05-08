#include "algoritmo.h"
#include <algorithm>

std::vector<Proceso> fifo(const std::vector<Proceso>& procesos) {
    std::vector<Proceso> orden = procesos;

    std::sort(orden.begin(), orden.end(), [](const Proceso& a, const Proceso& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    return orden;
}