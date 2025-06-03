#ifndef TIPOS_H
#define TIPOS_H

#include <QString>

// Estructura compartida para Bloques de Gantt (Simulación A)
struct BloqueGantt {
    QString pid;
    int inicio;
    int duracion;
};

#endif // TIPOS_H