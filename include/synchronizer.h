// include/synchronizer.h
#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include "proceso.h"
#include <string>
#include <vector>
#include <unordered_map>

/** Tipo de acción: lectura o escritura */
enum class ActionType { READ, WRITE };

/** Representa un recurso con su contador (1 = mutex, >1 = semáforo) */
struct Recurso {
    std::string name;
    int count;
};

/** Representa una acción de un proceso sobre un recurso en un ciclo dado */
struct Accion {
    std::string pid;
    ActionType type;
    std::string recurso;
    int cycle;
};

/** Bloque para la línea de tiempo: WAIT o ACCESS de un recurso */
struct BloqueSync {
    std::string pid;
    std::string recurso;
    int start;      // ciclo de inicio
    int duration;   // siempre 1 ciclo
    bool accessed;  // true = ACCESS, false = WAIT
};

/**
 * Carga procesos desde un archivo 
 */
std::vector<Proceso> loadProcesos(const std::string &path);

/**
 * Carga recursos desde un archivo:
 * Formato por línea: <NOMBRE>, <COUNT>
 */
std::vector<Recurso> loadRecursos(const std::string &path);

/**
 * Carga acciones desde un archivo:
 * Formato por línea: <PID>, <ACTION>, <RECURSO>, <CICLO>
 */
std::vector<Accion> loadAcciones(const std::string &path);

/**
 * Simula la sección B (mutex/semáforo) y devuelve bloques WAIT/ACCESS.
 * - `acciones` debe estar ordenado por cycle.
 * - `recursos` da el contador inicial de cada recurso.
 */
std::vector<BloqueSync> simulateSync(
    const std::vector<Accion> &acciones,
    const std::vector<Recurso> &recursos);

#endif // SYNCHRONIZER_H
