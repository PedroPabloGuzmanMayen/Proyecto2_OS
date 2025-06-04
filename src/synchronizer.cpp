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
                  if (A.cycle != B.cycle) {
                      return A.cycle < B.cycle;
                  }
                  return A.pid < B.pid;
              });
    return v;
}

void validateAndSortActions(std::vector<Accion> &acciones, 
                           const std::vector<Proceso> &procesos,
                           bool isMutex = false) {
    // Crear mapa de procesos para acceso rápido
    std::unordered_map<QString, Proceso> procesoMap;
    for (const auto &p : procesos) {
        procesoMap[p.pid] = p;
    }

    // Validar que cada acción tenga un ciclo >= arrival time del proceso
    for (const auto &accion : acciones) {
        auto it = procesoMap.find(accion.pid);
        if (it == procesoMap.end()) {
            QString error = QString("Proceso %1 no encontrado para acción en ciclo %2")
                           .arg(accion.pid).arg(accion.cycle);
            throw std::runtime_error(error.toStdString());
        }
        
        const Proceso &proceso = it->second;
        if (accion.cycle < proceso.arrivalTime) {
            QString error = QString("Error: Proceso %1 intenta acción en ciclo %2 pero su arrival time es %3")
                           .arg(accion.pid).arg(accion.cycle).arg(proceso.arrivalTime);
            throw std::runtime_error(error.toStdString());
        }
    }

    // Ordenar acciones por ciclo, luego por prioridad del proceso
    std::sort(acciones.begin(), acciones.end(),
              [&procesoMap](const Accion &A, const Accion &B) {
                  if (A.cycle != B.cycle) {
                      return A.cycle < B.cycle;
                  }
                  
                  // Si están en el mismo ciclo, ordenar por prioridad
                  // (menor número = mayor prioridad)
                  const Proceso &procA = procesoMap[A.pid];
                  const Proceso &procB = procesoMap[B.pid];
                  
                  if (procA.priority != procB.priority) {
                      return procA.priority < procB.priority;
                  }
                  
                  // Si tienen la misma prioridad, ordenar por PID para consistencia
                  return A.pid < B.pid;
              });
}

/**
 * Estado interno de cada acción para la simulación:
 * - pid: pid del proceso que hace la acción
 * - recurso: recurso que hace la acción.
 * - acción: acción realizada por el recuros
 * - cycle: ciclo en el que se realiza
 * - priroidad: prioridad en del procesi que hace la acción
 */
struct AccionEspera {
    QString pid;
    QString recurso;
    QString accionStr;
    int cycleSolicitado;
    int prioridad;
    
    bool operator<(const AccionEspera &other) const {
        // La prioridad se hace de mayor a menir
        if (prioridad != other.prioridad) {
            return prioridad > other.prioridad;
        }
        return pid > other.pid; // Desempate por PID
    }
};

/**
 * Estado interno de cada recurso para la simulación:
 * - capacity = cuántas instancias tiene (1 = mutex, >1 = semáforo).
 * - endTimes = min-heap de ciclos en que cada acceso termina (start + 1).
 */
struct ResState {
    int capacity = 0;
    int originalCapacity = 0; // Para logging
    std::priority_queue<int,
        std::vector<int>,
        std::greater<int>> endTimes;
    std::priority_queue<AccionEspera> waitingQueue; // Cola de procesos esperando
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
    const std::vector<Proceso> &procesos,
    std::vector<Recurso> &recursosVec, bool isMutex = false)
{
    // Crear mapa de procesos para acceso rápido a prioridades
    std::unordered_map<QString, Proceso> procesoMap;
    for (const auto &p : procesos) {
        procesoMap[p.pid] = p;
    }
    // Crear copia de acciones para validar y ordenar
    std::vector<Accion> accionesOrdenadas = acciones;
    validateAndSortActions(accionesOrdenadas, procesos, isMutex);
    //Mapa de recursos
    std::unordered_map<QString, ResState> resMap;
    for (const auto &r : recursosVec) {
        if (r.count <= 0) {
            qDebug() << "Recurso con capacidad inválida:" << r.name << r.count;
            continue;
        }
        ResState rs;
        rs.originalCapacity = r.count;
        // Si es mutex, limitar a 1 instancia independientemente del valor original
        rs.capacity = isMutex ? 1 : r.count;
        
        if (isMutex && r.count > 1) {
            qDebug() << "MUTEX: Recurso" << r.name << "limitado a 1 instancia (era" << r.count << ")";
        }
        
        resMap[r.name] = rs;
    }

    std::vector<BloqueSync> timeline;

    for (const auto &a : accionesOrdenadas) {

        // 1) Convertir el tipo de acción a cadena ("READ" o "WRITE")
        
        QString accionStr = (a.type == ActionType::READ ? "READ" : "WRITE");
        auto it = resMap.find(a.recurso);
        if (it == resMap.end()) {
            qDebug() << "Acción sobre recurso desconocido:" << a.recurso;
            continue;
        }
        auto &rs = it->second;

        // Obtener prioridad del proceso
        int prioridad = procesoMap[a.pid].priority;

        // 1) Liberar accesos terminados ANTES del ciclo actual
        while (!rs.endTimes.empty() && rs.endTimes.top() < a.cycle) {
            rs.endTimes.pop();
        }

        int used = rs.endTimes.size();
        int startAccess = a.cycle;

        // 2) Verificar si necesita esperar
        if (used >= rs.capacity) {
            // Agregar a la cola de espera
            rs.waitingQueue.push({a.pid, a.recurso, accionStr, a.cycle, prioridad});
            int nextFree = rs.endTimes.empty() ? a.cycle : rs.endTimes.top();
            if (nextFree > a.cycle) {
                // GENERAR WAIT:
                timeline.push_back({
                    a.pid,        // PID del proceso
                    a.recurso,      // Nombre del recurso
                    accionStr,      // "READ" o "WRITE"
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
            accionStr,     // "READ" o "WRITE"
            startAccess,  // Ciclo de inicio del acceso efectivo
            1,            // Duración = 1 ciclo para el ACCESS
            true          // true = ACCESS
        });
        // Registramos que esa plaza se liberará en (startAccess + 1)
        rs.endTimes.push(startAccess + 1);
    }

    return timeline;
}

std::vector<BloqueSync> simulateMutex(
    const std::vector<Accion> &acciones,
    std::vector<Recurso> &recursosVec,
    const std::vector<Proceso> &procesos)
{
    return simulateSync(acciones, procesos, recursosVec, true);
}

std::vector<BloqueSync> simulateSyncSemaforo(
    const std::vector<Accion> &acciones,
    std::vector<Recurso> &recursosVec,
    const std::vector<Proceso> &procesos)
{
    return simulateSync(acciones, procesos, recursosVec, false);
}