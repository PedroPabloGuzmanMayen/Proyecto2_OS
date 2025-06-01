#include "algoritmo.h"
#include <algorithm>
#include <queue>
#include <map>
#include "priorityQueue.h"

// ---------------------
// First In First Out
// ---------------------
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

// ---------------------
// Round Robin
// ---------------------
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
        // Encolar nuevos
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
        bloques.push_back({actual.pid, tiempo, ejecutar});
        tiempoRestante[actual.pid] -= ejecutar;
        tiempo += ejecutar;
        ultimaEjecucion[actual.pid] = tiempo;
        resultado.push_back(actual);

        // Encolar recién llegados
        while (i < procesos.size() && procesos[i].arrivalTime <= tiempo) {
            cola.push(procesos[i]);
            i++;
        }
        if (tiempoRestante[actual.pid] > 0)
            cola.push(actual);
    }

    // Calcular métricas finales
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

// ---------------------
// Shortest Job First
// ---------------------
std::vector<Proceso> shortestJobFirst(const std::vector<Proceso>& procesos) {
    auto pendientes = procesos;
    std::vector<Proceso> ejecucion;
    int tiempo = 0;

    // Orden inicial por arrivalTime
    std::sort(pendientes.begin(), pendientes.end(), [](auto& a, auto& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    while (!pendientes.empty()) {
        std::vector<Proceso*> ready;
        for (auto& p : pendientes)
            if (p.arrivalTime <= tiempo)
                ready.push_back(&p);

        if (ready.empty()) {
            tiempo = pendientes.front().arrivalTime;
            continue;
        }
        // Elegir el de menor burstTime
        auto cmpBurst = [](Proceso* a, Proceso* b) {
            return a->burstTime < b->burstTime;
        };
        auto elegido = *std::min_element(ready.begin(), ready.end(), cmpBurst);

        elegido->startTime = tiempo;
        elegido->completionTime = tiempo + elegido->burstTime;
        elegido->turnaroundTime = elegido->completionTime - elegido->arrivalTime;
        elegido->waitingTime = elegido->startTime - elegido->arrivalTime;
        tiempo = elegido->completionTime;

        ejecucion.push_back(*elegido);
        pendientes.erase(std::remove_if(pendientes.begin(), pendientes.end(),
                          [&](const Proceso& p){ return p.pid == elegido->pid; }),
                          pendientes.end());
    }
    return ejecucion;
}

// ---------------------
// Priority Scheduling
// ---------------------
std::vector<Proceso> priorityScheduling(const std::vector<Proceso>& procesos) {
    auto pendientes = procesos;
    std::vector<Proceso> ejecucion;
    int tiempo = 0;

    std::sort(pendientes.begin(), pendientes.end(), [](auto& a, auto& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    while (!pendientes.empty()) {
        std::vector<Proceso*> ready;
        for (auto& p : pendientes)
            if (p.arrivalTime <= tiempo)
                ready.push_back(&p);

        if (ready.empty()) {
            tiempo = pendientes.front().arrivalTime;
            continue;
        }
        // Elegir el de mayor prioridad (número menor = más alta prioridad)
        auto cmpPriority = [](Proceso* a, Proceso* b) {
            return a->priority < b->priority;
        };
        auto elegido = *std::min_element(ready.begin(), ready.end(), cmpPriority);

        elegido->startTime = tiempo;
        elegido->completionTime = tiempo + elegido->burstTime;
        elegido->turnaroundTime = elegido->completionTime - elegido->arrivalTime;
        elegido->waitingTime = elegido->startTime - elegido->arrivalTime;
        tiempo = elegido->completionTime;

        ejecucion.push_back(*elegido);
        pendientes.erase(std::remove_if(pendientes.begin(), pendientes.end(),
                          [&](const Proceso& p){ return p.pid == elegido->pid; }),
                          pendientes.end());
    }
    return ejecucion;
}

// -----------------------------------
// Shortest Remaining Time (SRT) – preemptive
// -----------------------------------
std::vector<Proceso> shortestRemainingTime(
    const std::vector<Proceso>& procesosOriginal,
    std::vector<BloqueGantt>& bloques)
{
    // 1) Copiar y ordenar por arrivalTime
    std::vector<Proceso> procesos = procesosOriginal;
    std::sort(procesos.begin(), procesos.end(),
              [](const Proceso& a, const Proceso& b) {
                  return a.arrivalTime < b.arrivalTime;
              });

    // 2) Mapas de estado
    std::map<QString,int> tiempoRestante;
    std::map<QString,int> primeraEjecucion;
    std::map<QString,int> ultimaEjecucion;
    for (const auto& p : procesos) {
        tiempoRestante[p.pid] = p.burstTime;
    }

    // 3) Cola mínima: priorityQueue por burstTime
    priorityQueue colaReady;

    int tiempo = 0;
    size_t i = 0;
    Proceso procesoActual;
    bool hayActual = false;
    int tickActual = 0;

    while (i < procesos.size() || !colaReady.isEmpty() || hayActual) {
        // 4) Encolar todos los que llegan en 'tiempo'
        while (i < procesos.size() && procesos[i].arrivalTime <= tiempo) {
            Proceso copia = procesos[i];
            copia.burstTime = tiempoRestante[copia.pid];
            colaReady.insertNewValue(copia);
            i++;
        }

        // 5) Preempción: si hay actual y aparece uno con remaining menor
        if (hayActual) {
            if (!colaReady.isEmpty()) {
                Proceso top = colaReady.top();
                if (top.burstTime < tiempoRestante[procesoActual.pid]) {
                    // Guardar bloque de Gantt
                    if (tickActual > 0) {
                        bloques.push_back({
                            procesoActual.pid,
                            tiempo - tickActual,
                            tickActual
                        });
                    }
                    // Reencolar si queda ráfaga
                    int rem = tiempoRestante[procesoActual.pid];
                    if (rem > 0) {
                        Proceso preemp = procesoActual;
                        preemp.burstTime = rem;
                        colaReady.insertNewValue(preemp);
                    }
                    hayActual = false;
                    tickActual = 0;
                }
            }
        }

        // 6) Si no hay actual y cola no está vacía, tomar uno nuevo
        if (!hayActual && !colaReady.isEmpty()) {
            Proceso siguiente = colaReady.pop();
            // Restaurar campos completos desde 'procesos'
            for (auto& p : procesos) {
                if (p.pid == siguiente.pid) {
                    procesoActual = p;
                    break;
                }
            }
            if (primeraEjecucion.find(procesoActual.pid) == primeraEjecucion.end()) {
                primeraEjecucion[procesoActual.pid] = tiempo;
            }
            hayActual = true;
            tickActual = 0;
        }

        // 7) Si no hay actual y no hay cola pero quedan por llegar, saltar
        if (!hayActual) {
            if (i < procesos.size()) {
                tiempo = procesos[i].arrivalTime;
                continue;
            } else {
                break;
            }
        }

        // 8) Ejecutar un ciclo
        tiempo++;
        tickActual++;
        tiempoRestante[procesoActual.pid]--;

        // 9) Si termina, cerrar bloque y registrar fin
        if (tiempoRestante[procesoActual.pid] == 0) {
            ultimaEjecucion[procesoActual.pid] = tiempo;
            bloques.push_back({
                procesoActual.pid,
                tiempo - tickActual,
                tickActual
            });
            hayActual = false;
            tickActual = 0;
        }
    }

    // 10) Construir vector final con todas las métricas
    std::vector<Proceso> final;
    for (auto p : procesosOriginal) {
        p.startTime = primeraEjecucion[p.pid];
        p.completionTime = ultimaEjecucion[p.pid];
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        final.push_back(p);
    }

    // 11) Ordenar por completionTime para que el test reciba la secuencia correcta
    std::sort(final.begin(), final.end(),
              [](const Proceso &a, const Proceso &b) {
                  return a.completionTime < b.completionTime;
              });

    return final;
}

// -----------------------------------
// Tiempo de espera promedio
// -----------------------------------
double calcularTiempoEsperaPromedio(
    const std::vector<Proceso>& procesosOriginal,
    const std::vector<Proceso>& ejecucion)
{
    double total = 0.0;
    for (const auto& p : ejecucion) {
        total += p.waitingTime;
    }
    return ejecucion.empty() ? 0.0 : total / ejecucion.size();
}