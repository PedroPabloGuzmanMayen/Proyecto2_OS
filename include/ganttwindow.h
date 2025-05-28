#ifndef GANTTWINDOW_H
#define GANTTWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QHBoxLayout>
#include <vector>
#include <map>
#include <QString>
#include <QColor>
#include "algoritmo.h"

class GanttWindow : public QWidget {
    Q_OBJECT

public:
    GanttWindow(const std::vector<BloqueGantt>& bloques, QWidget *parent = nullptr);

private slots:
    void mostrarSiguienteBloque();

private:
    std::vector<BloqueGantt> bloques;
    QHBoxLayout *layout;
    QTimer *timer;
    int indice;
    std::map<QString, QColor> colores;
};

#endif