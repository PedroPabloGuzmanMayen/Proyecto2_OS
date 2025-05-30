// src/synchronizer.cpp
#include "synchronizer.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <iostream>

// Trim de espacios en cabeza y cola
static std::string trim(const std::string &s) {
    auto b = s.find_first_not_of(" \t\r\n");
    auto e = s.find_last_not_of(" \t\r\n");
    return (b == std::string::npos) ? "" : s.substr(b, e - b + 1);
}

// --- Parsers ---

std::vector<Proceso> loadProcesos(const std::string &path) {
    std::vector<Proceso> v;
    std::ifstream f(path);
    if (!f) {
        std::cerr << "No se pudo abrir procesos en: " << path << "\n";
        return v;
    }
    std::string line;
    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string pidStr;
        int bt, at, pr;
        char comma;
        // Leemos hasta la coma, luego los ints
        if (std::getline(ss, pidStr, ',') &&
            ss >> bt >> comma >> at >> comma >> pr)
        {
            Proceso p;
            p.pid         = QString::fromStdString(trim(pidStr));
            p.burstTime   = bt;
            p.arrivalTime = at;
            p.priority    = pr;
            v.push_back(p);
        } else {
            std::cerr << "Línea procesos mal formateada: " << line << "\n";
        }
    }
    return v;
}

std::vector<Recurso> loadRecursos(const std::string &path) {
    std::vector<Recurso> v;
    std::ifstream f(path);
    if (!f) {
        std::cerr << "No se pudo abrir recursos en: " << path << "\n";
        return v;
    }
    std::string line;
    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string name;
        int cnt;
        char comma;
        if (std::getline(ss, name, ',') && ss >> cnt) {
            v.push_back({ trim(name), cnt });
        } else {
            std::cerr << "Línea recursos mal formateada: " << line << "\n";
        }
    }
    return v;
}

std::vector<Accion> loadAcciones(const std::string &path) {
    std::vector<Accion> v;
    std::ifstream f(path);
    if (!f) {
        std::cerr << "No se pudo abrir acciones en: " << path << "\n";
        return v;
    }
    std::string line;
    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string pidStr, typeStr, recStr;
        int cycle;
        // Se lee tres campos hasta coma y luego el ciclo
        if (std::getline(ss, pidStr, ',') &&
            std::getline(ss, typeStr, ',') &&
            std::getline(ss, recStr, ',') &&
            ss >> cycle)
        {
            Accion a;
            a.pid     = trim(pidStr);
            a.type    = (trim(typeStr) == "READ"
                         ? ActionType::READ
                         : ActionType::WRITE);
            a.recurso = trim(recStr);
            a.cycle   = cycle;
            v.push_back(a);
        } else {
            std::cerr << "Línea acciones mal formateada: " << line << "\n";
        }
    }
    // Ordenamos por ciclo
    std::sort(v.begin(), v.end(),
              [](auto &A, auto &B){ return A.cycle < B.cycle; });
    return v;
}

// --- Simulación ---

// Estado interno de cada recurso
struct ResState {
    int capacity = 0;
    std::priority_queue<int,
        std::vector<int>,
        std::greater<int>> endTimes; // min-heap de tiempos de fin
};

std::vector<BloqueSync> simulateSync(
    const std::vector<Accion> &acciones,
    const std::vector<Recurso> &recursosVec)
{
    // Construimos un mapa recurso→estado
    std::unordered_map<std::string, ResState> resMap;
    for (const auto &r : recursosVec) {
        resMap[r.name].capacity = r.count;
    }

    std::vector<BloqueSync> timeline;

    for (const auto &a : acciones) {
        auto it = resMap.find(a.recurso);
        if (it == resMap.end()) {
            std::cerr << "Acción sobre recurso desconocido: "
                      << a.recurso << "\n";
            continue;
        }
        auto &rs = it->second;

        // Liberar accesos ya finalizados
        while (!rs.endTimes.empty() && rs.endTimes.top() <= a.cycle)
            rs.endTimes.pop();

        int used = rs.endTimes.size();
        int startAccess = a.cycle;

        // Si se supera la capacidad, esperamos hasta nextFree
        if (rs.capacity > 0 && used >= rs.capacity) {
            int nextFree = rs.endTimes.empty()
                         ? a.cycle
                         : rs.endTimes.top();
            if (nextFree > a.cycle) {
                // Bloque WAIT
                timeline.push_back({
                    a.pid,
                    a.recurso,
                    a.cycle,
                    nextFree - a.cycle,
                    false
                });
            }
            // Usamos esa plaza
            if (!rs.endTimes.empty())
                rs.endTimes.pop();
            startAccess = nextFree;
        }

        // Bloque ACCESS (duración = 1 ciclo)
        timeline.push_back({
            a.pid,
            a.recurso,
            startAccess,
            1,
            true
        });
        // Programamos fin de acceso
        rs.endTimes.push(startAccess + 1);
    }

    return timeline;
}