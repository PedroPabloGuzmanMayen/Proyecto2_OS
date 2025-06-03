#ifndef GANTTWINDOW_H
#define GANTTWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QMap>
#include <QString>
#include <QColor>
#include <QLabel>
#include <vector>
#include "tipos.h" // Para que BloqueGantt esté disponible

class GanttWindow : public QWidget {
    Q_OBJECT

public:
    explicit GanttWindow(QWidget *parent = nullptr);
    ~GanttWindow();

    void actualizarNumeroCiclo(int ciclo);

    void agregarBloqueEnTiempoReal(const QString &pid, int ciclo);

    void limpiarDiagrama();

private:
    // Etiqueta que mostrará “Ciclo: X”
    QLabel *labelCiclo;            

    // Vector original de bloques lógicos (cada uno con pid, inicio y duracion)
    std::vector<BloqueGantt> bloques;
    // Índice del BloqueGantt actual que se está procesando
    int indiceBloque;
    // Contador de cuántos bloques unitarios hemos dibujado del bloque lógico actual
    int unidadActual;

    // Mapa PID → color asignado (para que cada proceso conserve siempre el mismo color)
    QMap<QString, QColor> colores;

    // ===== Widgets y layouts para presentar el Gantt =====
    // Contenedor principal de bloques (un QWidget con layout horizontal)
    QWidget     *contenedorBloques;
    // Layout horizontal donde se irán agregando los bloques unitarios
    QHBoxLayout *layoutBloques;
    // ScrollArea que envuelve a contenedorBloques y permite el scroll horizontal
    QScrollArea *scrollArea;

    // Temporizador que dispara cada “tick” para animar el diagrama
    QTimer *timer;

    // Tamaño base (en píxeles) de cada unidad de tiempo: ancho × alto
    static constexpr int ANCHO_BASE_UNIDAD = 20;
    static constexpr int ALTO_UNIDAD       = 40;

    // Retorna (o asigna si no existía) un color único para el PID dado.
    QColor colorParaPID(const QString &pid);
};

#endif // GANTTWINDOW_H