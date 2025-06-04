#include "synchronizer.h"
#include "proceso.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <QMessageBox>
#include <QWidget>
#include <QThread>
#include <QApplication>
#include "ganttwindow.h"

// Función de delay
void delaySync(int milisegundos) {
    QThread::msleep(milisegundos);
    QApplication::processEvents(); // Procesar eventos de la GUI
}

/**
 * Carga procesos desde archivo. Cada línea con formato:
 *   <PID>,<BT>,<AT>,<Priority>
 */
std::vector<Proceso> loadProcesos(const QString &ruta) {
    std::vector<Proceso> v;
    std::unordered_set<QString> seenPIDs;
    QStringList errores;  // acumulador de mensajes

    QFile f(ruta);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error",
                             QString("No se pudo abrir procesos en:\n%1").arg(ruta));
        return v;
    }
    QTextStream in(&f);
    int lineaNum = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineaNum++;
        if (line.isEmpty()) continue;

        QStringList partes = line.split(",");
        if (partes.size() >= 4) {
            QString pidStr    = partes[0].trimmed();
            bool    okBurst   = false;
            bool    okArrival = false;
            bool    okPrio    = false;
            int     burstVal   = partes[1].trimmed().toInt(&okBurst);
            int     arrivalVal = partes[2].trimmed().toInt(&okArrival);
            int     prioVal    = partes[3].trimmed().toInt(&okPrio);

            // 1) Validar PID no vacío
            if (pidStr.isEmpty()) {
                errores.append(QString("PID vacío en línea %1 : \"%2\"")
                              .arg(lineaNum).arg(line));
                continue;
            }
            // 2) Validar burstTime > 0
            if (!okBurst || burstVal <= 0) {
                errores.append(QString("BurstTime inválido en línea %1 : \"%2\"")
                              .arg(lineaNum).arg(partes[1].trimmed()));
                continue;
            }
            // 3) Validar arrivalTime >= 0
            if (!okArrival || arrivalVal < 0) {
                errores.append(QString("ArrivalTime inválido en línea %1 : \"%2\"")
                              .arg(lineaNum).arg(partes[2].trimmed()));
                continue;
            }
            // 4) Validar priority >= 0
            if (!okPrio || prioVal < 0) {
                errores.append(QString("Priority inválido en línea %1 : \"%2\"")
                              .arg(lineaNum).arg(partes[3].trimmed()));
                continue;
            }
            // 5) Detectar PID duplicado
            if (seenPIDs.find(pidStr) != seenPIDs.end()) {
                errores.append(QString("Proceso duplicado detectado: PID \"%1\" en línea %2")
                              .arg(pidStr).arg(lineaNum));
                continue;
            }

            // Crear y agregar el proceso válido
            Proceso p;
            p.pid            = pidStr;
            p.burstTime      = burstVal;
            p.arrivalTime    = arrivalVal;
            p.priority       = prioVal;
            p.startTime      = -1;
            p.completionTime = -1;
            p.waitingTime    = 0;
            p.turnaroundTime = 0;

            v.push_back(p);
            seenPIDs.insert(pidStr);
        } else {
            errores.append(QString("Línea procesos mal formateada en línea %1 : \"%2\"")
                          .arg(lineaNum).arg(line));
        }
    }
    f.close();

    if (!errores.isEmpty()) {
        QMessageBox::warning(nullptr, "Errores en procesos.txt", errores.join("\n"));
        return {};
    }
    return v;
}

/**
 * Carga recursos desde archivo. Cada línea con formato:
 *   <NOMBRE>,<COUNT>
 */
std::vector<Recurso> loadRecursos(const QString &ruta) {
    std::vector<Recurso> v;
    std::unordered_set<QString> seenRecursos;
    QStringList errores;  // acumulador de mensajes

    QFile f(ruta);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error",
                             QString("No se pudo abrir recursos en:\n%1").arg(ruta));
        return v;
    }
    QTextStream in(&f);
    int lineaNum = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineaNum++;
        if (line.isEmpty()) continue;

        QStringList partes = line.split(",");
        if (partes.size() >= 2) {
            QString nombreRec = partes[0].trimmed();
            bool    okCount   = false;
            int     countVal  = partes[1].trimmed().toInt(&okCount);

            // 1) Validar nombre no vacío
            if (nombreRec.isEmpty()) {
                errores.append(QString("Nombre de recurso vacío en línea %1").arg(lineaNum));
                continue;
            }
            // 2) Validar count es entero válido
            if (!okCount) {
                errores.append(QString("Cantidad de recurso inválida en línea %1 : \"%2\"")
                              .arg(lineaNum).arg(partes[1].trimmed()));
                continue;
            }
            // 3) Validar count >= 0
            if (countVal < 0) {
                errores.append(QString("Cantidad negativa para recurso \"%1\" en línea %2")
                              .arg(nombreRec).arg(lineaNum));
                continue;
            }
            // 4) Detectar duplicados de recurso
            if (seenRecursos.find(nombreRec) != seenRecursos.end()) {
                errores.append(QString("Recurso duplicado detectado: \"%1\" en línea %2")
                              .arg(nombreRec).arg(lineaNum));
                continue;
            }

            // Crear y agregar recurso válido
            Recurso r;
            r.name  = nombreRec;
            r.count = countVal;
            v.push_back(r);
            seenRecursos.insert(nombreRec);
        } else {
            errores.append(QString("Línea recursos mal formateada en línea %1 : \"%2\"")
                          .arg(lineaNum).arg(line));
        }
    }
    f.close();

    if (!errores.isEmpty()) {
        QMessageBox::warning(nullptr, "Errores en recursos.txt", errores.join("\n"));
        return {};
    }
    return v;
}

/**
 * Carga acciones desde archivo. Cada línea con formato:
 *   <PID>,<ACTION>,<RECURSO>,<CICLO>
 */
std::vector<Accion> loadAcciones(const QString &ruta) {
    std::vector<Accion> v;
    std::unordered_set<QString> seenAcciones;
    QStringList errores;  // acumulador de mensajes

    QFile f(ruta);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error",
                             QString("No se pudo abrir acciones en:\n%1").arg(ruta));
        return v;
    }
    QTextStream in(&f);
    int lineaNum = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineaNum++;
        if (line.isEmpty()) continue;

        QStringList partes = line.split(",");
        if (partes.size() >= 4) {
            QString pidStr    = partes[0].trimmed();
            QString actionStr = partes[1].trimmed().toUpper();
            QString recName   = partes[2].trimmed();
            bool    okCycle   = false;
            int     cycleVal  = partes[3].trimmed().toInt(&okCycle);

            // 1) Validar que PID no esté vacío
            if (pidStr.isEmpty()) {
                errores.append(QString("PID vacío en línea %1 : \"%2\"").arg(lineaNum).arg(line));
                continue;
            }
            // 2) Validar que ACTION sea “READ” o “WRITE”
            ActionType tipoAccion;
            if (actionStr == "READ") {
                tipoAccion = ActionType::READ;
            } else if (actionStr == "WRITE") {
                tipoAccion = ActionType::WRITE;
            } else {
                errores.append(QString("Tipo de acción no reconocido en línea %1 : \"%2\"")
                              .arg(lineaNum).arg(actionStr));
                continue;
            }
            // 3) Validar que recurso no esté vacío
            if (recName.isEmpty()) {
                errores.append(QString("Recurso vacío en línea %1 : \"%2\"").arg(lineaNum).arg(line));
                continue;
            }
            // 4) Validar que ciclo sea entero ≥ 0
            if (!okCycle || cycleVal < 0) {
                errores.append(QString("Ciclo inválido en línea %1 : \"%2\"")
                              .arg(lineaNum).arg(partes[3].trimmed()));
                continue;
            }

            // Construir la acción preliminar
            Accion a;
            a.pid     = pidStr;
            a.type    = tipoAccion;
            a.recurso = recName;
            a.cycle   = cycleVal;

            // Generar clave única: "PID|R/W|Recurso|Ciclo"
            QString clave = a.pid + "|" +
                            (a.type == ActionType::READ ? "R" : "W") + "|" +
                            a.recurso + "|" +
                            QString::number(a.cycle);

            // 5) Verificar duplicados exactos
            if (seenAcciones.find(clave) != seenAcciones.end()) {
                errores.append(QString("Acción duplicada detectada en línea %1 : PID=\"%2\" ACTION=\"%3\" RECURSO=\"%4\" CICLO=%5")
                              .arg(lineaNum)
                              .arg(pidStr)
                              .arg(actionStr)
                              .arg(recName)
                              .arg(cycleVal));
                continue;
            }

            // Agregar acción válida
            v.push_back(a);
            seenAcciones.insert(clave);
        } else {
            errores.append(QString("Línea acciones mal formateada en línea %1 : \"%2\"")
                          .arg(lineaNum).arg(line));
        }
    }
    f.close();

    if (!errores.isEmpty()) {
        QMessageBox::warning(nullptr, "Errores en acciones.txt", errores.join("\n"));
        return {};
    }

    // Ordenar acciones por ciclo y luego por PID
    std::sort(v.begin(), v.end(),
              [](const Accion &A, const Accion &B) {
                  if (A.cycle != B.cycle) {
                      return A.cycle < B.cycle;
                  }
                  return A.pid < B.pid;
              });
    return v;
}

bool validateAndSortActions(std::vector<Accion> &acciones, 
                            const std::vector<Proceso> &procesos,
                            QWidget *parent /*= nullptr*/,
                            bool isMutex /*= false*/) {
    // Crear mapa de procesos para acceso rápido
    std::unordered_map<QString, Proceso> procesoMap;
    for (const auto &p : procesos) {
        procesoMap[p.pid] = p;
    }
    // Lista para acumular errores
    QStringList errores;

    // Validar que cada acción tenga un ciclo >= arrivalTime del proceso
    for (const auto &accion : acciones) {
        auto it = procesoMap.find(accion.pid);
        if (it == procesoMap.end()) {
            errores.append(QString("Proceso %1 no encontrado para acción en ciclo %2")
                          .arg(accion.pid).arg(accion.cycle));
            continue;
        }
        const Proceso &proceso = it->second;
        if (accion.cycle < proceso.arrivalTime) {
            errores.append(QString("Proceso %1 intenta acción en ciclo %2 pero su arrival time es %3")
                          .arg(accion.pid).arg(accion.cycle).arg(proceso.arrivalTime));
        }
    }

    if (!errores.isEmpty()) {
        QString mensajeCompleto = "Se encontraron los siguientes errores de validación:\n\n";
        mensajeCompleto += errores.join("\n");
        mensajeCompleto += "\n\nLa simulación no puede continuar.";

        if (parent != nullptr) {
            QMessageBox::critical(parent, 
                                  "Error de Validación - Sincronizador", 
                                  mensajeCompleto);
        } else {
            QMessageBox::critical(nullptr, 
                                  "Error de Validación - Sincronizador", 
                                  mensajeCompleto);
        }
        return false;
    }

    // Ordenar acciones por ciclo, luego por prioridad del proceso
    std::sort(acciones.begin(), acciones.end(),
              [&procesoMap](const Accion &A, const Accion &B) {
                  if (A.cycle != B.cycle) {
                      return A.cycle < B.cycle;
                  }
                  const Proceso &procA = procesoMap[A.pid];
                  const Proceso &procB = procesoMap[B.pid];
                  if (procA.priority != procB.priority) {
                      return procA.priority < procB.priority;
                  }
                  return A.pid < B.pid;
              });

    return true;
}

/**
 * Estado interno de cada acción para la simulación:
 * - pid: pid del proceso que hace la acción
 * - recurso: recurso que hace la acción.
 * - acción: acción realizada por el recuros
 * - cycle: ciclo en el que se realiza
 * - prioridad: prioridad del proceso que hace la acción
 */
struct AccionEspera {
    QString pid;
    QString recurso;
    QString accionStr;
    int     cycleSolicitado;
    int     prioridad;
    
    bool operator<(const AccionEspera &other) const {
        // La prioridad se ordena de mayor a menor
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
    std::vector<Recurso> &recursosVec,
    bool isMutex,
    GanttWindow *gantt,
    QWidget *parent)
{
    // Crear mapa de procesos para acceso rápido a prioridades
    std::unordered_map<QString, Proceso> procesoMap;
    for (const auto &p : procesos) {
        procesoMap[p.pid] = p;
    }
    // Crear copia de acciones para validar y ordenar
    std::vector<Accion> accionesOrdenadas = acciones;
    // Validar acciones antes de continuar
    if (!validateAndSortActions(accionesOrdenadas, procesos, parent, isMutex)) {
        qDebug() << "SIMULACIÓN ABORTADA: Errores en la validación de acciones";
        return std::vector<BloqueSync>(); // Retornar vector vacío en caso de error
    }
    // Mapa de recursos
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
        QString accionStr = (a.type == ActionType::READ ? "READ" : "WRITE");
        auto it = resMap.find(a.recurso);
        if (it == resMap.end()) {
            qDebug() << "Acción sobre recurso desconocido:" << a.recurso;
            continue;
        }
        auto &rs = it->second;

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
            // Generar BloqueSync de WAIT
            BloqueSync waitBlock = {
                a.pid,
                a.recurso,
                accionStr,
                a.cycle,
                nextFree - a.cycle, // Duración de la espera
                false // false = WAIT
            };
            timeline.push_back(waitBlock);
            // Dibujar WAIT
            if (gantt != nullptr) {
                for (int offset = 0; offset < waitBlock.duration; ++offset) {
                    int cicloActual = waitBlock.start + offset;
                    gantt->agregarBloqueSync(
                        waitBlock.pid,
                        waitBlock.recurso,
                        waitBlock.accion,
                        cicloActual,
                        false // WAIT
                    );
                    delaySync(500);
                }
            }

            startAccess = nextFree;
            rs.endTimes.pop();
        }

        // Generar BloqueSync de ACCESS
        BloqueSync accessBlock = {
            a.pid,
            a.recurso,
            accionStr,
            startAccess,
            1, // Duración = 1 ciclo
            true // true = ACCESS
        };
        timeline.push_back(accessBlock);

        // Dibujar ACCESS
        if (gantt != nullptr) {
            gantt->agregarBloqueSync(
                accessBlock.pid,
                accessBlock.recurso,
                accessBlock.accion,
                startAccess,
                true // ACCESS
            );
            delaySync(3000);
        }

        // Registrar fin de acceso en (startAccess + 1)
        rs.endTimes.push(startAccess + 1);
    }

    return timeline;
}

std::vector<BloqueSync> simulateMutex(
    const std::vector<Accion> &acciones,
    std::vector<Recurso> &recursosVec,
    const std::vector<Proceso> &procesos,
    GanttWindow *gantt,
    QWidget *parent)
{
    return simulateSync(acciones, procesos, recursosVec, true, gantt, parent);
}

std::vector<BloqueSync> simulateSyncSemaforo(
    const std::vector<Accion> &acciones,
    std::vector<Recurso> &recursosVec,
    const std::vector<Proceso> &procesos,
    GanttWindow *gantt,
    QWidget *parent)
{
    return simulateSync(acciones, procesos, recursosVec, false, gantt, parent);
}