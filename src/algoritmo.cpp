#include "algoritmo.h"
#include <algorithm>
#include <queue>
#include <map>

std::vector<Proceso> fifo(const std::vector<Proceso>& procesos) {
    std::vector<Proceso> resultado = procesos;

    std::sort(resultado.begin(), resultado.end(), [](const Proceso& a, const Proceso& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int tiempo = 0;

    for (auto& p : resultado) {
        if (tiempo < p.arrivalTime)
            tiempo = p.arrivalTime;

        p.startTime = tiempo;
        p.completionTime = tiempo + p.burstTime;
        p.waitingTime = p.startTime - p.arrivalTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;

        tiempo = p.completionTime;
    }

    return resultado;
}

std::vector<Proceso> roundRobin(const std::vector<Proceso>& procesosOriginal, int quantum, std::vector<BloqueGantt>& bloques) {
    std::vector<Proceso> resultado;
    std::vector<Proceso> procesos = procesosOriginal;

    std::sort(procesos.begin(), procesos.end(), [](const Proceso& a, const Proceso& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    std::queue<Proceso> cola;
    std::map<QString, int> tiempoRestante;
    std::map<QString, int> primeraEjecucion;
    std::map<QString, int> ultimaEjecucion;

    for (const auto& p : procesos)
        tiempoRestante[p.pid] = p.burstTime;

    int tiempo = 0;
    size_t i = 0;

    while (!cola.empty() || i < procesos.size()) {
        while (i < procesos.size() && procesos[i].arrivalTime <= tiempo) {
            cola.push(procesos[i]);
            i++;
        }

        if (cola.empty()) {
            tiempo++;
            continue;
        }

        Proceso actual = cola.front();
        cola.pop();

        if (primeraEjecucion.find(actual.pid) == primeraEjecucion.end())
            primeraEjecucion[actual.pid] = tiempo;

        int ejecutar = std::min(quantum, tiempoRestante[actual.pid]);
        bloques.push_back({actual.pid, tiempo, ejecutar});  // ← Aquí se añade el bloque
        tiempoRestante[actual.pid] -= ejecutar;
        tiempo += ejecutar;
        ultimaEjecucion[actual.pid] = tiempo;

        resultado.push_back(actual);

        while (i < procesos.size() && procesos[i].arrivalTime <= tiempo) {
            cola.push(procesos[i]);
            i++;
        }

        if (tiempoRestante[actual.pid] > 0) {
            cola.push(actual);
        }
    }

    std::vector<Proceso> final;
    for (auto p : procesosOriginal) {
        p.startTime = primeraEjecucion[p.pid];
        p.completionTime = ultimaEjecucion[p.pid];
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        final.push_back(p);
    }

    return final;
}

double calcularTiempoEsperaPromedio(const std::vector<Proceso>& procesosOriginal, const std::vector<Proceso>& ejecucion) {
    double total = 0.0;
    for (const auto& p : ejecucion)
        total += p.waitingTime;

    return ejecucion.empty() ? 0.0 : total / ejecucion.size();
}