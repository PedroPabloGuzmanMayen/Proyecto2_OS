#include "ganttwindow.h"
#include <QLabel>
#include <QFrame>
#include <QColor>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>

GanttWindow::GanttWindow(const std::vector<BloqueGantt>& bloques, QWidget *parent)
    : QWidget(parent), bloques(bloques), indice(0) {

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    layout = new QHBoxLayout();
    layout->setSpacing(4);

    QLabel *titulo = new QLabel("Diagrama de Gantt", this);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-weight: bold; font-size: 16px;");
    mainLayout->addWidget(titulo);

    QHBoxLayout *leyendaLayout = new QHBoxLayout();
    for (const auto& b : bloques) {
        if (colores.find(b.pid) == colores.end()) {
            QColor color = QColor::fromHsv(QRandomGenerator::global()->bounded(360), 255, 200);
            colores[b.pid] = color;

            QLabel *leyenda = new QLabel(b.pid);
            leyenda->setFixedWidth(50);
            leyenda->setAlignment(Qt::AlignCenter);
            leyenda->setStyleSheet(QString("background-color: %1; border: 1px solid gray;")
                                   .arg(color.name()));
            leyendaLayout->addWidget(leyenda);
        }
    }
    mainLayout->addLayout(leyendaLayout);

    QWidget *bloquesContainer = new QWidget();
    bloquesContainer->setLayout(layout);
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(bloquesContainer);
    mainLayout->addWidget(scrollArea);

    setLayout(mainLayout);
    setWindowTitle("Diagrama de Gantt");
    resize(600, 200);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GanttWindow::mostrarSiguienteBloque);
    timer->start(500);
}

void GanttWindow::mostrarSiguienteBloque() {
    if (indice >= bloques.size()) {
        timer->stop();
        return;
    }

    const BloqueGantt& b = bloques[indice++];
    QLabel *bloque = new QLabel(QString("%1\n%2-%3").arg(b.pid).arg(b.inicio).arg(b.inicio + b.duracion), this);
    bloque->setAlignment(Qt::AlignCenter);
    bloque->setFixedSize(b.duracion * 20, 40);  // Escalado horizontal
    bloque->setFrameShape(QFrame::Box);
    bloque->setStyleSheet(QString("background-color: %1; border: 1px solid black;")
                          .arg(colores[b.pid].name()));

    layout->addWidget(bloque);
}