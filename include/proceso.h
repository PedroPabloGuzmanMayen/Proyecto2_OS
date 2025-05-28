#ifndef PROCESO_H
#define PROCESO_H

#include <QString>
#include <vector>

struct Proceso {
    QString pid;        // Identificador del proceso
    int burstTime;      // Tiempo de ejecución
    int arrivalTime;    // Tiempo de llegada
    int priority;       // Prioridad (si aplica)

    // Campos adicionales para cálculo de tiempos
    int startTime = -1;         // Tiempo en que inicia por primera vez
    int completionTime = -1;    // Tiempo en que finaliza
    int waitingTime = -1;       // Tiempo de espera
    int turnaroundTime = -1;    // Tiempo de retorno (espera + burstTime)
};

std::vector<Proceso> cargarProcesosDesdeArchivo(const QString &ruta);

#endif