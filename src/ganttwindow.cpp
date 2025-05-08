#include "ganttwindow.h"
#include <QLabel>
#include <QFrame>
#include <QColor>
#include <QRandomGenerator>

GanttWindow::GanttWindow(const std::vector<Proceso>& ejecucion, QWidget *parent)
    : QWidget(parent), ejecucion(ejecucion), indice(0) {

    layout = new QHBoxLayout(this);
    layout->setSpacing(2);

    setLayout(layout);
    setWindowTitle("Diagrama de Gantt");

    // Asignar color único a cada proceso
    for (const auto& p : ejecucion) {
        if (colores.find(p.pid) == colores.end()) {
            QColor color = QColor::fromHsv(QRandomGenerator::global()->bounded(360), 255, 200);
            colores[p.pid] = color;
        }
    }

    // Timer para animación paso a paso
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GanttWindow::mostrarSiguienteBloque);
    timer->start(500);  // muestra un bloque cada 500ms
}

void GanttWindow::mostrarSiguienteBloque() {
    if (indice >= ejecucion.size()) {
        timer->stop();
        return;
    }

    const Proceso& p = ejecucion[indice++];
    QLabel *bloque = new QLabel(p.pid, this);
    bloque->setAlignment(Qt::AlignCenter);
    bloque->setFixedSize(40, 40);
    bloque->setFrameShape(QFrame::Box);
    bloque->setStyleSheet(QString("background-color: %1;").arg(colores[p.pid].name()));

    layout->addWidget(bloque);
}