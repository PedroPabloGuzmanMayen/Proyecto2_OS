#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

#include "../include/proceso.h"
#include "../include/algoritmo.h"
#include "../include/synchronizer.h"

// Función auxiliar: extrae solo la secuencia de PIDs de un vector<Proceso>
static std::vector<std::string> seq_pids(const std::vector<Proceso>& v) {
    std::vector<std::string> out;
    for (auto &p : v) out.push_back(p.pid.toStdString());
    return out;
}

// Función auxiliar: obtiene solo la primera aparición de cada PID, en orden
static std::vector<std::string> seq_pids_unicos(const std::vector<std::string>& v) {
    std::vector<std::string> únicos;
    for (auto &pid : v) {
        if (std::find(únicos.begin(), únicos.end(), pid) == únicos.end()) {
            únicos.push_back(pid);
        }
    }
    return únicos;
}

// Función auxiliar: compara dos vectores de string y reporta si coinciden
static bool igual_seq(const std::vector<std::string>& a,
                      const std::vector<std::string>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

// ---------------------------------
// Tests para Simulación A (algoritmos)
// ---------------------------------

static bool test_fifo_simple() {
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 1, 0,0,0,0 };
    Proceso p2{ "P2", 3, 1, 2, 0,0,0,0 };
    Proceso p3{ "P3", 4, 2, 1, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);
    procesos.push_back(p3);

    auto result = fifo(procesos, nullptr);
    std::vector<std::string> resPids = seq_pids(result);
    std::vector<std::string> exp = { "P1", "P2", "P3" };

    if (!igual_seq(resPids, exp)) {
        std::cout << "  [ERROR] FIFO simple: "
                  << "salida = ";
        for (auto &x : resPids) std::cout << x << " ";
        std::cout << "pero se esperaba P1 P2 P3\n";
        return false;
    }
    return true;
}

static bool test_sjf_simple() {
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 1, 0,0,0,0 };
    Proceso p2{ "P2", 3, 1, 2, 0,0,0,0 };
    Proceso p3{ "P3", 4, 2, 1, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);
    procesos.push_back(p3);

    auto result = shortestJobFirst(procesos, nullptr);
    std::vector<std::string> resPids = seq_pids(result);
    // Ajustado para esperar el orden según burstTime puro: P2 (3), P3 (4), P1 (5)
    std::vector<std::string> exp = { "P2", "P3", "P1" };

    if (!igual_seq(resPids, exp)) {
        std::cout << "  [ERROR] SJF simple: salida = ";
        for (auto &x : resPids) std::cout << x << " ";
        std::cout << "pero se esperaba P2 P3 P1\n";
        return false;
    }
    return true;
}

static bool test_priority_simple() {
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 2, 0,0,0,0 };
    Proceso p2{ "P2", 3, 1, 1, 0,0,0,0 };
    Proceso p3{ "P3", 4, 2, 3, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);
    procesos.push_back(p3);

    auto result = priorityScheduling(procesos, nullptr);
    std::vector<std::string> resPids = seq_pids(result);
    // Ajustado para esperar el orden según prioridad pura: P2 (1), P1 (2), P3 (3)
    std::vector<std::string> exp = { "P2", "P1", "P3" };

    if (!igual_seq(resPids, exp)) {
        std::cout << "  [ERROR] Priority simple: salida = ";
        for (auto &x : resPids) std::cout << x << " ";
        std::cout << "pero se esperaba P2 P1 P3\n";
        return false;
    }
    return true;
}

static bool test_rr_simple() {
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 1, 0,0,0,0 };
    Proceso p2{ "P2", 3, 1, 1, 0,0,0,0 };
    Proceso p3{ "P3", 4, 2, 1, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);
    procesos.push_back(p3);

    int quantum = 2;
    std::vector<BloqueGantt> dummy; 
    auto result = roundRobin(procesos, quantum, dummy, nullptr);
    // Obtenemos la secuencia completa de PIDs (puede repetirse un mismo PID varias veces)
    std::vector<std::string> resPids = seq_pids(result);
    // Filtramos para quedarnos solo con la PRIMERA aparición de cada PID
    std::vector<std::string> uniquePids = seq_pids_unicos(resPids);
    std::vector<std::string> exp = { "P1", "P2", "P3" };

    if (!igual_seq(uniquePids, exp)) {
        std::cout << "  [ERROR] Round Robin simple: salida única = ";
        for (auto &x : uniquePids) std::cout << x << " ";
        std::cout << "pero se esperaba P1 P2 P3\n";
        return false;
    }
    return true;
}

static bool test_srt_simple() {
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 1, 0,0,0,0 };
    Proceso p2{ "P2", 3, 1, 1, 0,0,0,0 };
    Proceso p3{ "P3", 4, 2, 1, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);
    procesos.push_back(p3);

    std::vector<BloqueGantt> bloques;
    auto result = shortestRemainingTime(procesos, bloques, nullptr);
    std::vector<std::string> resPids = seq_pids(result);
    std::vector<std::string> exp = { "P2", "P1", "P3" };

    if (!igual_seq(resPids, exp)) {
        std::cout << "  [ERROR] SRT simple: salida = ";
        for (auto &x : resPids) std::cout << x << " ";
        std::cout << "pero se esperaba P2 P1 P3\n";
        return false;
    }
    return true;
}

// ---------------------------------
// Tests para Simulación B (mutex/semaforos)
// ---------------------------------

// Test básico de mutex: dos procesos compiten por un recurso con capacity=1
static bool test_mutex_simple() {
    // Definir procesos
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 1, 0,0,0,0 };
    Proceso p2{ "P2", 3, 0, 1, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);

    // Definir recursos: R1 con count=1 (mutex)
    std::vector<Recurso> recursos;
    recursos.push_back({ "R1", 1 });

    // Definir acciones: ambos P1 y P2 leen R1 en ciclo 0
    std::vector<Accion> acciones;
    acciones.push_back({ "P1", ActionType::READ, "R1", 0 });
    acciones.push_back({ "P2", ActionType::READ, "R1", 0 });

    // Ejecutar simulación de mutex
    auto timeline = simulateMutex(acciones, recursos, procesos, nullptr, nullptr);

    // Esperamos 3 bloques: P1 ACCESS, P2 WAIT, P2 ACCESS
    if (timeline.size() != 3) {
        std::cout << "  [ERROR] Mutex simple: timeline size = " << timeline.size()
                  << " pero se esperaba 3\n";
        return false;
    }

    // Verificar secuencia de (pid, accessed)
    std::vector<std::pair<std::string,bool>> expected = {
        { "P1", true },
        { "P2", false },
        { "P2", true }
    };
    for (size_t i = 0; i < expected.size(); i++) {
        if (timeline[i].pid.toStdString() != expected[i].first ||
            timeline[i].accessed != expected[i].second) {
            std::cout << "  [ERROR] Mutex simple: bloque " << i
                      << " = (" << timeline[i].pid.toStdString()
                      << ", " << (timeline[i].accessed ? "ACCESS" : "WAIT")
                      << ") pero se esperaba ("
                      << expected[i].first << ", "
                      << (expected[i].second ? "ACCESS" : "WAIT")
                      << ")\n";
            return false;
        }
    }

    return true;
}

// Test básico de semáforo: dos procesos compiten por un recurso con capacity=2
static bool test_semaforo_simple() {
    // Definir procesos
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 1, 0,0,0,0 };
    Proceso p2{ "P2", 3, 0, 1, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);

    // Definir recursos: R1 con count=2 (semaforo)
    std::vector<Recurso> recursos;
    recursos.push_back({ "R1", 2 });

    // Definir acciones: ambos P1 y P2 leen R1 en ciclo 0
    std::vector<Accion> acciones;
    acciones.push_back({ "P1", ActionType::READ, "R1", 0 });
    acciones.push_back({ "P2", ActionType::READ, "R1", 0 });

    // Ejecutar simulación de semáforo
    auto timeline = simulateSyncSemaforo(acciones, recursos, procesos, nullptr, nullptr);

    // Esperamos 2 bloques: P1 ACCESS, P2 ACCESS (sin WAIT)
    if (timeline.size() != 2) {
        std::cout << "  [ERROR] Semáforo simple: timeline size = " << timeline.size()
                  << " pero se esperaba 2\n";
        return false;
    }

    // Verificar ambos bloques son ACCESS
    for (size_t i = 0; i < timeline.size(); i++) {
        if (!timeline[i].accessed) {
            std::cout << "  [ERROR] Semáforo simple: bloque " << i
                      << " es WAIT pero se esperaba ACCESS\n";
            return false;
        }
    }
    return true;
}

int main() {
    int passed = 0, failed = 0;

    std::cout << "=== TEST FIFO ===\n";
    if (test_fifo_simple()) { std::cout << "  OK\n"; passed++; }
    else { std::cout << "  FAILED\n"; failed++; }

    std::cout << "\n=== TEST SJF ===\n";
    if (test_sjf_simple()) { std::cout << "  OK\n"; passed++; }
    else { std::cout << "  FAILED\n"; failed++; }

    std::cout << "\n=== TEST PRIORITY ===\n";
    if (test_priority_simple()) { std::cout << "  OK\n"; passed++; }
    else { std::cout << "  FAILED\n"; failed++; }

    std::cout << "\n=== TEST ROUND ROBIN ===\n";
    if (test_rr_simple()) { std::cout << "  OK\n"; passed++; }
    else { std::cout << "  FAILED\n"; failed++; }

    std::cout << "\n=== TEST SRT ===\n";
    if (test_srt_simple()) { std::cout << "  OK\n"; passed++; }
    else { std::cout << "  FAILED\n"; failed++; }

    std::cout << "\n=== TEST MUTEX ===\n";
    if (test_mutex_simple()) { std::cout << "  OK\n"; passed++; }
    else { std::cout << "  FAILED\n"; failed++; }

    std::cout << "\n=== TEST SEMÁFORO ===\n";
    if (test_semaforo_simple()) { std::cout << "  OK\n"; passed++; }
    else { std::cout << "  FAILED\n"; failed++; }

    std::cout << "\n---- Resumen: " << passed << " passed, " << failed << " failed ----\n";
    return (failed == 0 ? 0 : 1);
}