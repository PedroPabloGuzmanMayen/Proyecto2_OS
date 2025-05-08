#ifndef GANTTWINDOW_H
#define GANTTWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QHBoxLayout>
#include <vector>
#include <map>
#include "proceso.h"

class GanttWindow : public QWidget {
    Q_OBJECT

public:
    GanttWindow(const std::vector<Proceso>& ejecucion, QWidget *parent = nullptr);

private slots:
    void mostrarSiguienteBloque();

private:
    std::vector<Proceso> ejecucion;
    QHBoxLayout *layout;
    QTimer *timer;
    int indice;
    std::map<QString, QColor> colores;
};

#endif