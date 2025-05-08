#include "algoritmo.h"
#include <algorithm>
#include <queue>
#include <map>

std::vector<Proceso> fifo(const std::vector<Proceso>& procesos) {
    std::vector<Proceso> orden = procesos;

    std::sort(orden.begin(), orden.end(), [](const Proceso& a, const Proceso& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    return orden;
}

std::vector<Proceso> roundRobin(const std::vector<Proceso>& procesosOriginal, int quantum) {
    std::vector<Proceso> resultado;
    std::vector<Proceso> procesos = procesosOriginal;

    std::sort(procesos.begin(), procesos.end(), [](const Proceso& a, const Proceso& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    std::queue<Proceso> cola;
    int tiempo = 0;
    size_t i = 0;

    std::map<QString, int> bt_restantes;
    for (const auto& p : procesos)
        bt_restantes[p.pid] = p.burstTime;

    while (i < procesos.size() || !cola.empty()) {
        // Encolar procesos que han llegado
        while (i < procesos.size() && procesos[i].arrivalTime <= tiempo) {
            cola.push(procesos[i]);
            i++;
        }

        if (cola.empty()) {
            tiempo++; // avanzar si no hay proceso listo
            continue;
        }

        Proceso actual = cola.front();
        cola.pop();

        int restante = bt_restantes[actual.pid];
        int ejecutar = std::min(restante, quantum);

        // Ejecutar el proceso
        bt_restantes[actual.pid] -= ejecutar;
        tiempo += ejecutar;

        resultado.push_back(actual);  // Se guarda cada aparición en orden de ejecución

        // Encolar nuevos procesos que llegaron durante ejecución
        while (i < procesos.size() && procesos[i].arrivalTime <= tiempo) {
            cola.push(procesos[i]);
            i++;
        }

        // Si todavía le queda tiempo de CPU, lo reencolamos
        if (bt_restantes[actual.pid] > 0) {
            cola.push(actual);
        }
    }

    return resultado;
}

double calcularTiempoEsperaPromedio(const std::vector<Proceso>& procesosOriginal, const std::vector<Proceso>& ejecucion) {
    std::map<QString, int> arrivalMap;
    for (const auto& p : procesosOriginal)
        arrivalMap[p.pid] = p.arrivalTime;

    std::map<QString, int> primeraEjecucion;
    int tiempo = 0;

    for (const auto& p : ejecucion) {
        if (primeraEjecucion.find(p.pid) == primeraEjecucion.end()) {
            primeraEjecucion[p.pid] = tiempo;
        }
        tiempo++;
    }

    double totalEspera = 0.0;
    for (const auto& [pid, inicio] : primeraEjecucion) {
        totalEspera += inicio - arrivalMap[pid];
    }

    return totalEspera / procesosOriginal.size();
}