#include "algoritmo.h"
#include <algorithm>
#include <queue>
#include <map>
#include "priorityQueue.h"

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

// ------------------
// Shortest Job First
// ------------------
std::vector<Proceso> shortestJobFirst(const std::vector<Proceso>& procesos) {
    // Lista mutable de procesos pendientes
    auto pendientes = procesos;
    std::vector<Proceso> ejecucion;
    int tiempo = 0;

    // Orden inicial por llegada
    std::sort(pendientes.begin(), pendientes.end(),
              [](auto& a, auto& b){ return a.arrivalTime < b.arrivalTime; });

    while (!pendientes.empty()) {
        // Filtrar los que ya han llegado
        std::vector<Proceso*> ready;
        for (auto& p : pendientes)
            if (p.arrivalTime <= tiempo)
                ready.push_back(&p);

        if (ready.empty()) {
            // si no hay ninguno listo, avanzar al siguiente arrivalTime
            tiempo = pendientes.front().arrivalTime;
            continue;
        }

        // Elegir el de menor burstTime
        auto cmpBurst = [](Proceso* a, Proceso* b) {
            return a->burstTime < b->burstTime;
        };
        auto elegido = *std::min_element(ready.begin(), ready.end(), cmpBurst);

        // Ejecutar
        elegido->startTime = tiempo;
        elegido->completionTime = tiempo + elegido->burstTime;
        elegido->turnaroundTime = elegido->completionTime - elegido->arrivalTime;
        elegido->waitingTime = elegido->startTime - elegido->arrivalTime;
        tiempo = elegido->completionTime;

        // Añadir a la lista de resultado y remover de pendientes
        ejecucion.push_back(*elegido);
        pendientes.erase(std::remove_if(pendientes.begin(), pendientes.end(),
                          [&](const Proceso& p){ return p.pid == elegido->pid; }),
                          pendientes.end());
    }

    return ejecucion;
}

// -------------------
// Priority Scheduling 
// -------------------
std::vector<Proceso> priorityScheduling(const std::vector<Proceso>& procesos) {
    auto pendientes = procesos;
    std::vector<Proceso> ejecucion;
    int tiempo = 0;

    // Orden inicial por llegada
    std::sort(pendientes.begin(), pendientes.end(),
              [](auto& a, auto& b){ return a.arrivalTime < b.arrivalTime; });

    while (!pendientes.empty()) {
        // Filtrar los que ya han llegado
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

        // Ejecutar
        elegido->startTime = tiempo;
        elegido->completionTime = tiempo + elegido->burstTime;
        elegido->turnaroundTime = elegido->completionTime - elegido->arrivalTime;
        elegido->waitingTime = elegido->startTime - elegido->arrivalTime;
        tiempo = elegido->completionTime;

        // Añadir y remover
        ejecucion.push_back(*elegido);
        pendientes.erase(std::remove_if(pendientes.begin(), pendientes.end(),
                          [&](const Proceso& p){ return p.pid == elegido->pid; }),
                          pendientes.end());
    }

    return ejecucion;
}
// -------------------
// Shortest remaining time 
// -------------------
std::vector<Proceso> shortestRemainingTime(const std::vector<Proceso>& procesosOriginal, std::vector<BloqueGantt>& bloques) {
    std::vector<Proceso> procesos = procesosOriginal;
    std::vector<Proceso> resultado;
    
    // Ordenar por tiempo de llegada
    std::sort(procesos.begin(), procesos.end(), [](const Proceso& a, const Proceso& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    priorityQueue colaReady;
    std::map<QString, int> tiempoRestante;
    std::map<QString, int> primeraEjecucion;
    std::map<QString, int> ultimaEjecucion;
    
    // Inicializarla estructura que guarda los tiempos restatntes 
    for (const auto& p : procesos) {
        tiempoRestante[p.pid] = p.burstTime;
    }

    int tiempo = 0;
    size_t i = 0;
    Proceso* procesoActual = nullptr;
    int tiempoEjecucionActual = 0;

    while (!colaReady.isEmpty() || i < procesos.size() || procesoActual != nullptr) {
        // Agregar procesos que han llegado a la cola de ready
        while (i < procesos.size() && procesos[i].arrivalTime <= tiempo) {
            Proceso nuevo = procesos[i];
            nuevo.burstTime = tiempoRestante[nuevo.pid]; // Usar tiempo restante como prioridad
            colaReady.insertNewValue(nuevo);
            i++;
        }

        // Si hay un proceso ejecutándose
        if (procesoActual != nullptr) {
            // Verificar si hay un proceso con menor tiempo restante que el actual
            if (!colaReady.isEmpty()) {
                Proceso siguienteProceso = colaReady.top();
                
                // Si el siguiente proceso tiene menos tiempo restante, hacer preemption
                if (siguienteProceso.burstTime < tiempoRestante[procesoActual->pid]) {
                    // Guardar el bloque del proceso actual
                    if (tiempoEjecucionActual > 0) {
                        bloques.push_back({procesoActual->pid, tiempo - tiempoEjecucionActual, tiempoEjecucionActual});
                    }
                    
                    // Devolver el proceso actual a la cola si no ha terminado
                    if (tiempoRestante[procesoActual->pid] > 0) {
                        Proceso procesoPreemptado = *procesoActual;
                        procesoPreemptado.burstTime = tiempoRestante[procesoActual->pid];
                        colaReady.insertNewValue(procesoPreemptado);
                    }
                    
                    procesoActual = nullptr;
                    tiempoEjecucionActual = 0;
                }
            }
        }

        // Si no hay proceso ejecutándose, tomar el siguiente de la cola
        if (procesoActual == nullptr && !colaReady.isEmpty()) {
            Proceso siguiente = colaReady.pop();
            procesoActual = new Proceso(siguiente);
            
            // Buscar el proceso original para obtener todos sus datos
            for (auto& p : procesos) {
                if (p.pid == siguiente.pid) {
                    *procesoActual = p;
                    break;
                }
            }
            
            // Registrar primera ejecución si es necesario
            if (primeraEjecucion.find(procesoActual->pid) == primeraEjecucion.end()) {
                primeraEjecucion[procesoActual->pid] = tiempo;
            }
            
            tiempoEjecucionActual = 0;
        }

        // Si no hay ningún proceso ready, avanzar el tiempo
        if (procesoActual == nullptr) {
            if (i < procesos.size()) {
                tiempo = procesos[i].arrivalTime;
            } else {
                break;
            }
            continue;
        }

        // Ejecutar el proceso actual por 1 unidad de tiempo
        tiempo++;
        tiempoEjecucionActual++;
        tiempoRestante[procesoActual->pid]--;

        // Si el proceso terminó
        if (tiempoRestante[procesoActual->pid] == 0) {
            ultimaEjecucion[procesoActual->pid] = tiempo;
            
            // Guardar el último bloque
            bloques.push_back({procesoActual->pid, tiempo - tiempoEjecucionActual, tiempoEjecucionActual});
            
            delete procesoActual;
            procesoActual = nullptr;
            tiempoEjecucionActual = 0;
        }
    }

    // Limpiar memoria si queda algún proceso
    if (procesoActual != nullptr) {
        delete procesoActual;
    }

    // Calcular tiempos finales para cada proceso
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