#ifndef PROCESO_H
#define PROCESO_H

#include <string>
#include <vector>

struct Proceso {
    std::string pid;
    int burstTime;
    int arrivalTime;
    int priority;
};

std::vector<Proceso> cargarProcesosDesdeArchivo(const std::string& ruta);

#endif