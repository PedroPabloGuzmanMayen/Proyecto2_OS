#include "synchronizer.h"
#include "proceso.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <algorithm>
#include <queue>
#include <unordered_map>

/**
 * Carga procesos desde archivo. Cada línea con formato:
 *   <PID>,<BT>,<AT>,<Priority>
 */
std::vector<Proceso> loadProcesos(const QString &ruta) {
    std::vector<Proceso> v;
    QFile f(ruta);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir procesos en:" << ruta;
        return v;
    }
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList partes = line.split(",");
        if (partes.size() >= 4) {
            Proceso p;
            p.pid         = partes[0].trimmed();
            p.burstTime   = partes[1].trimmed().toInt();
            p.arrivalTime = partes[2].trimmed().toInt();
            p.priority    = partes[3].trimmed().toInt();
            v.push_back(p);
        } else {
            qDebug() << "Línea procesos mal formateada:" << line;
        }
    }
    f.close();
    return v;
}

/**
 * Carga recursos desde archivo. Cada línea con formato:
 *   <NOMBRE>,<COUNT>
 */
std::vector<Recurso> loadRecursos(const QString &ruta) {
    std::vector<Recurso> v;
    QFile f(ruta);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir recursos en:" << ruta;
        return v;
    }
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList partes = line.split(",");
        if (partes.size() >= 2) {
            Recurso r;
            r.name  = partes[0].trimmed();
            r.count = partes[1].trimmed().toInt();
            v.push_back(r);
        } else {
            qDebug() << "Línea recursos mal formateada:" << line;
        }
    }
    f.close();
    return v;
}

/**
 * Carga acciones desde archivo. Cada línea con formato:
 *   <PID>,<ACTION>,<RECURSO>,<CICLO>
 */
std::vector<Accion> loadAcciones(const QString &ruta) {
    std::vector<Accion> v;
    QFile f(ruta);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir acciones en:" << ruta;
        return v;
    }
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList partes = line.split(",");
        if (partes.size() >= 4) {
            Accion a;
            a.pid     = partes[0].trimmed();
            QString typeStr = partes[1].trimmed().toUpper();
            a.type    = (typeStr == "READ") ? ActionType::READ : ActionType::WRITE;
            a.recurso = partes[2].trimmed();
            a.cycle   = partes[3].trimmed().toInt();
            v.push_back(a);
        } else {
            qDebug() << "Línea acciones mal formateada:" << line;
        }
    }
    f.close();

    // Ordenar acciones por ciclo ascendente
    std::sort(v.begin(), v.end(),
              [](const Accion &A, const Accion &B) {
                  return A.cycle < B.cycle;
              });
    return v;
}

/**
 * Estado interno de cada recurso para la simulación:
 * - capacity = cuántas instancias tiene (1 = mutex, >1 = semáforo).
 * - endTimes = min-heap de ciclos en que cada acceso termina (start + 1).
 */
struct ResState {
    int capacity = 0;
    std::priority_queue<int,
        std::vector<int>,
        std::greater<int>> endTimes;
};

/**
 * Simula la sección B (mutex/semáforo):
 * - `acciones`: vector de Accion ordenado por ciclo.
 * - `recursosVec`: vector inicial de Recursos con su contador.
 *
 * Para cada acción:
 *   1) Libera accesos finalizados en el ciclo actual (pop de endTimes).
 *   2) Si `used >= capacity`, genera un BloqueSync de WAIT y ajusta el ciclo de acceso
 *      al `nextFree` antes de hacer ACCESS.
 *   3) Siempre genera un BloqueSync de ACCESS (duration=1) en `startAccess` y programa
 *      su fin en `startAccess + 1` (push a endTimes).
 */
std::vector<BloqueSync> simulateSync(
    const std::vector<Accion> &acciones,
    std::vector<Recurso> &recursosVec)
{
    std::unordered_map<QString, ResState> resMap;
    for (const auto &r : recursosVec) {
        if (r.count <= 0) {
            qDebug() << "Recurso con capacidad inválida:" << r.name << r.count;
            continue;
        }
        resMap[r.name].capacity = r.count;
    }

    std::vector<BloqueSync> timeline;

    for (const auto &a : acciones) {
        auto it = resMap.find(a.recurso);
        if (it == resMap.end()) {
            qDebug() << "Acción sobre recurso desconocido:" << a.recurso;
            continue;
        }
        auto &rs = it->second;

        // 1) Liberar accesos terminados ANTES del ciclo actual
        while (!rs.endTimes.empty() && rs.endTimes.top() < a.cycle) {
            rs.endTimes.pop();
        }

        int used = rs.endTimes.size();
        int startAccess = a.cycle;

        // 2) Verificar si necesita esperar
        if (used >= rs.capacity) {
            int nextFree = rs.endTimes.empty() ? a.cycle : rs.endTimes.top();
            if (nextFree > a.cycle) {
                // GENERAR WAIT:
                timeline.push_back({
                    a.pid,        // PID del proceso
                    a.recurso,    // Nombre del recurso
                    a.cycle,      // Inicio de la espera (ciclo original)
                    nextFree - a.cycle, // Duración de la espera
                    false         // false = WAIT
                });
                // Actualizamos startAccess para que el ACCESS ocurra en nextFree
                startAccess = nextFree;
                // Liberamos la plaza que se va a usar
                rs.endTimes.pop();
            }
        }

        // Una vez pase la espera (o si no hubo que esperar), generamos el bloque ACCESS
        timeline.push_back({
            a.pid,        // PID
            a.recurso,    // Recurso
            startAccess,  // Ciclo de inicio del acceso efectivo
            1,            // Duración = 1 ciclo para el ACCESS
            true          // true = ACCESS
        });
        // Registramos que esa plaza se liberará en (startAccess + 1)
        rs.endTimes.push(startAccess + 1);
    }

    return timeline;
}

std::vector<BloqueSync> simulateSyncSemaforo(
    const std::vector<Accion> &acciones,
    std::vector<Recurso> &recursosVec)
{
    
    return simulateSync(acciones, recursosVec);
}