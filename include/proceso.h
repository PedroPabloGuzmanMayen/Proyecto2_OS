#ifndef PROCESO_H
#define PROCESO_H

#include <QString>
#include <vector>

struct Proceso {
    QString pid;
    int burstTime;
    int arrivalTime;
    int priority;
    int startTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
};

std::vector<Proceso> cargarProcesosDesdeArchivo(const QString &ruta);

#endif
