#include "ganttwindow.h"
#include <QLabel>
#include <QFrame>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>

GanttWindow::GanttWindow(QWidget *parent)
    : QWidget(parent)
{
    // 1) Layout principal vertical
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(10);


    // 2) Crear y añadir la etiqueta de “Ciclo: X”
    labelCiclo = new QLabel("Ciclo: 0", this);          // etiqueta inicial
    labelCiclo->setAlignment(Qt::AlignCenter);
    labelCiclo->setStyleSheet("font-weight: bold; font-size: 14px; margin: 4px;");
    mainLayout->addWidget(labelCiclo);      

    // 3) Título centrado
    QLabel *titulo = new QLabel("Diagrama de Gantt", this);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-weight: bold; font-size: 18px; color: #333;");
    mainLayout->addWidget(titulo);

    contenedorBloques = new QWidget(this); //Scroll area en donde guardamos las componentes del diagrama de Gantt
    contenedorBloques->setContentsMargins(0, 0, 0, 0);

    layoutBloques = new QHBoxLayout(contenedorBloques);
    layoutBloques->setSpacing(0);                 // Sin separación horizontal
    layoutBloques->setContentsMargins(0, 0, 0, 0); // Sin márgenes internos

    

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidget(contenedorBloques);
    scrollArea->setContentsMargins(0, 0, 0, 0);

    scrollArea->setStyleSheet(
        "QScrollArea { background-color: #fafafa; border: 1px solid #ccc; }"
    );

    mainLayout->addWidget(scrollArea, 1);
}

GanttWindow::~GanttWindow() {

}

// Implementación de actualizarNumeroCiclo
void GanttWindow::actualizarNumeroCiclo(int ciclo) {   // implementamos el método
    if (labelCiclo) {
        labelCiclo->setText(QString("Ciclo: %1").arg(ciclo));
    }
}

// Devuelve el color asociado a un PID, asignándolo si aún no existía
QColor GanttWindow::colorParaPID(const QString &pid) {
    if (colores.contains(pid)) {
        return colores.value(pid);
    }
    QColor c = QColor::fromHsv(QRandomGenerator::global()->bounded(360), 200, 200);
    colores.insert(pid, c);
    return c;
}

void GanttWindow::agregarBloqueEnTiempoReal(const QString &pid, int ciclo) {

    actualizarNumeroCiclo(ciclo);                     // actualiza “Ciclo: X”
    
    QColor colorProceso = colorParaPID(pid);

    QLabel *bloqueUnitario = new QLabel(QString("%1\n%2").arg(pid).arg(ciclo), contenedorBloques);
    bloqueUnitario->setFixedSize(ANCHO_BASE_UNIDAD, ALTO_UNIDAD);
    bloqueUnitario->setAlignment(Qt::AlignCenter);
    bloqueUnitario->setFrameShape(QFrame::NoFrame);
    bloqueUnitario->setStyleSheet(QString(
        "background-color: %1; color: white; font-size: 9px; font-weight: bold; "
        "border: 1px solid #444; margin: 0; padding: 0;")
        .arg(colorProceso.name()));
    bloqueUnitario->setToolTip(QString("Proceso: %1, Ciclo: %2").arg(pid).arg(ciclo));

    layoutBloques->addWidget(bloqueUnitario);
    
    // Auto-scroll al final
    QScrollBar *hbar = scrollArea->horizontalScrollBar();
    QTimer::singleShot(50, [hbar]() {
        hbar->setValue(hbar->maximum());
    });
}

void GanttWindow::limpiarDiagrama() {
    // Eliminar todos los widgets del layout
    QLayoutItem *item;
    while ((item = layoutBloques->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    colores.clear();
}