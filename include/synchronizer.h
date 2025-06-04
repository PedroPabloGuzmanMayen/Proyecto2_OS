#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include "proceso.h"
#include <QString>
#include <vector>
#include "tipos.h" // Para usar BloqueSync

/** Tipo de acción: lectura o escritura */
enum class ActionType { READ, WRITE };

/** Representa un recurso con su contador (1 = mutex, >1 = semáforo) */
struct Recurso {
    QString name;
    int count;
};

/** Representa una acción de un proceso sobre un recurso en un ciclo dado */
struct Accion {
    QString pid;
    ActionType type;
    QString recurso;
    int cycle;
};

/** Bloque para la línea de tiempo: WAIT o ACCESS de un recurso */
struct BloqueSync {
    QString pid;
    QString recurso;
    QString accion; // "READ" o "WRITE"
    int start;      // ciclo de inicio
    int duration;   // siempre 1 ciclo
    bool accessed;  // true = ACCESS, false = WAIT
};

/**
 * Carga procesos desde un archivo (usando Proceso definido en proceso.h).
 * - Formato de cada línea en <ruta>:
 *     <PID>,<BT>,<AT>,<Priority>
 */
std::vector<Proceso> loadProcesos(const QString &ruta);

/**
 * Carga recursos desde un archivo:
 * - Formato de cada línea:
 *     <NOMBRE>,<COUNT>
 */
std::vector<Recurso> loadRecursos(const QString &ruta);

/**
 * Carga acciones desde un archivo:
 * - Formato de cada línea:
 *     <PID>,<ACTION>,<RECURSO>,<CICLO>
 */
std::vector<Accion> loadAcciones(const QString &ruta);

/**
 * Simula la sección B (mutex/semáforo) y devuelve bloques WAIT/ACCESS.
 * - `acciones` debe estar ordenado por cycle ascendente.
 * - `recursos` proporciona el contador inicial de cada recurso.
 */

std::vector<BloqueSync> simulateSync(
    const std::vector<Accion> &acciones,
    const std::vector<Proceso> &procesos,
    std::vector<Recurso> &recursosVec, bool isMutex);

std::vector<BloqueSync> simulateMutex(
    const std::vector<Accion> &acciones,
    std::vector<Recurso> &recursosVec,
    const std::vector<Proceso> &procesos);

std::vector<BloqueSync> simulateSyncSemaforo(
    const std::vector<Accion> &acciones,
    std::vector<Recurso> &recursosVec,
    const std::vector<Proceso> &procesos);


#endif // SYNCHRONIZER_H