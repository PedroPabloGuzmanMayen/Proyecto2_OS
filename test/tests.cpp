#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

#include "../include/proceso.h"
#include "../include/algoritmo.h"

// Función auxiliar: extrae solo la secuencia de PIDs de un vector<Proceso>
static std::vector<std::string> seq_pids(const std::vector<Proceso>& v) {
    std::vector<std::string> out;
    for (auto &p : v) out.push_back(p.pid.toStdString());
    return out;
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

// Test FIFO con caso sencillo:
// Procesos:
//   P1, BT=5, AT=0, Prio=1
//   P2, BT=3, AT=1, Prio=2
//   P3, BT=4, AT=2, Prio=1
//
// Orden esperado (por AT): P1→P2→P3
//    - P1 arranca en 0, termina en 5
//    - P2 arranca en 5, termina en 8
//    - P3 arranca en 8, termina en 12
static bool test_fifo_simple() {
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 1, 0,0,0,0 };
    Proceso p2{ "P2", 3, 1, 2, 0,0,0,0 };
    Proceso p3{ "P3", 4, 2, 1, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);
    procesos.push_back(p3);

    auto result = fifo(procesos);
    std::vector<std::string> resPids = seq_pids(result);
    std::vector<std::string> exp = { "P1", "P2", "P3" };

    if (!igual_seq(resPids, exp)) {
        std::cout << "  [ERROR] FIFO simple: "
                  << " salida = ";
        for (auto &x : resPids) std::cout << x << " ";
        std::cout << "  pero se esperaba P1 P2 P3\n";
        return false;
    }
    return true;
}

// Test SJF (non-preemptive) con el mismo conjunto de procesos:
//   P1 (BT=5, AT=0), P2 (BT=3, AT=1), P3 (BT=4, AT=2).
// En SJF esperábamos:
//   - En tiempo 0 solo P1 está listo → P1 ejecuta (0-5).
//   - En tiempo 5, P2 (BT=3, AT=1) y P3 (BT=4, AT=2) están listos.
//     Se elige P2 (BT=3 < BT=4) → P2 ejecuta (5-8).
//   - Finalmente P3 (8-12).
static bool test_sjf_simple() {
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 1, 0,0,0,0 };
    Proceso p2{ "P2", 3, 1, 2, 0,0,0,0 };
    Proceso p3{ "P3", 4, 2, 1, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);
    procesos.push_back(p3);

    auto result = shortestJobFirst(procesos);
    std::vector<std::string> resPids = seq_pids(result);
    std::vector<std::string> exp = { "P1", "P2", "P3" };

    if (!igual_seq(resPids, exp)) {
        std::cout << "  [ERROR] SJF simple: salida = ";
        for (auto &x : resPids) std::cout << x << " ";
        std::cout << "  pero se esperaba P1 P2 P3\n";
        return false;
    }
    return true;
}

// Test Priority Scheduling (non-preemptive).
// Usamos los mismos procesos, pero con prioridades distintas:
//   P1 (BT=5, AT=0, Prio=2), P2 (BT=3, AT=1, Prio=1), P3 (BT=4, AT=2, Prio=3)
// En Priority, en t=0 solo P1 listo (prioridad 2) → P1 (0-5).
// En t=5, P2 (prio=1) y P3 (prio=3) están listos. Se elige P2 (prio=1 < 3).
static bool test_priority_simple() {
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 2, 0,0,0,0 };
    Proceso p2{ "P2", 3, 1, 1, 0,0,0,0 };
    Proceso p3{ "P3", 4, 2, 3, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);
    procesos.push_back(p3);

    auto result = priorityScheduling(procesos);
    std::vector<std::string> resPids = seq_pids(result);
    std::vector<std::string> exp = { "P1", "P2", "P3" };

    if (!igual_seq(resPids, exp)) {
        std::cout << "  [ERROR] Priority simple: salida = ";
        for (auto &x : resPids) std::cout << x << " ";
        std::cout << "  pero se esperaba P1 P2 P3\n";
        return false;
    }
    return true;
}

// Test Round Robin con quantum = 2.
// Procesos (igual AT, BT, Prio, pero no importan prioridades para RR):
//   P1(5,0), P2(3,1), P3(4,2).
// RR (quantum=2) produce orden de ejec. con reentrenamientos:
//   t=0-2: P1(queda 3)
//   t=2-3: P2 (queda 1) (P2 llegó en t=1)
//   t=3-5: P3 (queda 2) (P3 llegó en t=2)
//   t=5-7: P1 (queda 1)
//   t=7-8: P2 (queda 0)  → P2 termina
//   t=8-10: P3 (queda 0) → P3 termina
//   t=10-11: P1 (queda 0) → P1 termina
// Secuencia completa de “quién arranca cada quantum”: P1, P2, P3, P1, P2, P3, P1.
// Pero si la función `roundRobin(...)` devuelve solo un vector ordenado de salida
// (sin duplicados) igual que los demás, esperamos [P1, P2, P3].
static bool test_rr_simple() {
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 1, 0,0,0,0 };
    Proceso p2{ "P2", 3, 1, 1, 0,0,0,0 };
    Proceso p3{ "P3", 4, 2, 1, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);
    procesos.push_back(p3);

    int quantum = 2;
    std::vector<BloqueGantt> dummy; // bloqu es de salida interna, pero no nos importa aquí
    auto result = roundRobin(procesos, quantum, dummy);
    std::vector<std::string> resPids = seq_pids(result);
    std::vector<std::string> exp = { "P1", "P2", "P3" };

    if (!igual_seq(resPids, exp)) {
        std::cout << "  [ERROR] Round Robin simple: salida = ";
        for (auto &x : resPids) std::cout << x << " ";
        std::cout << "  pero se esperaba P1 P2 P3\n";
        return false;
    }
    return true;
}

// Test SRT (Shortest Remaining Time, preemptive).
// Usamos P1(5,0), P2(3,1), P3(4,2). En t=0, P1 arranca, pero en t=1 llega P2 (BT=3 < resto de P1=4)
//   → P1 es preempted, P2 arranca (1-4). En t=2 llega P3 (BT=4 > resto de P2=2), P2 sigue.
//   En t=4 P2 completa, luego en t=4-8: entre P1(resto=4) y P3(BT=4), desempata quizá por fifo (P1 primero).
//   Secuencia final de “primer arranque”: P1, P2, P1, P3. Pero la función “shortestRemainingTime” debe
//   devolver vector<Proceso> en orden de terminación o en orden de “arranque”? En nuestra impl,
//   devuelve en orden de llegada a completion. Por consistencia con los demás tests, asumimos
//   que la salida es el orden en que cada proceso finaliza: [P2, P1, P3] (P2 finaliza en t=4, P1 en t=8, P3 en t=12).
static bool test_srt_simple() {
    std::vector<Proceso> procesos;
    Proceso p1{ "P1", 5, 0, 1, 0,0,0,0 };
    Proceso p2{ "P2", 3, 1, 1, 0,0,0,0 };
    Proceso p3{ "P3", 4, 2, 1, 0,0,0,0 };
    procesos.push_back(p1);
    procesos.push_back(p2);
    procesos.push_back(p3);

    std::vector<BloqueGantt> bloques;
    auto result = shortestRemainingTime(procesos, bloques);
    std::vector<std::string> resPids = seq_pids(result);
    std::vector<std::string> exp = { "P2", "P1", "P3" };

    if (!igual_seq(resPids, exp)) {
        std::cout << "  [ERROR] SRT simple: salida = ";
        for (auto &x : resPids) std::cout << x << " ";
        std::cout << "  pero se esperaba P2 P1 P3\n";
        return false;
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

    std::cout << "\n---- Resumen: " << passed << " passed, " << failed << " failed ----\n";
    return (failed == 0 ? 0 : 1);
}