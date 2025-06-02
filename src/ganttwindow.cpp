#include "ganttwindow.h"
#include <QLabel>
#include <QFrame>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

GanttWindow::GanttWindow(const std::vector<BloqueGantt>& bloques, QWidget *parent)
    : QWidget(parent),
      bloques(bloques),
      indiceBloque(0),
      unidadActual(0)
{
    // 1) Layout principal vertical
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(10);

    // 2) Título centrado
    QLabel *titulo = new QLabel("Diagrama de Gantt", this);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-weight: bold; font-size: 18px; color: #333;");
    mainLayout->addWidget(titulo);

    // 3) Leyenda de colores (PID → rectángulo coloreado)
    QHBoxLayout *leyendaLayout = new QHBoxLayout();
    leyendaLayout->setSpacing(6);
    leyendaLayout->setContentsMargins(0, 0, 0, 0);

    for (const auto &b : bloques) {
        if (!colores.contains(b.pid)) {
            // Asignamos un color aleatorio por PID
            QColor c = QColor::fromHsv(QRandomGenerator::global()->bounded(360), 200, 200);
            colores.insert(b.pid, c);

            QLabel *lbl = new QLabel(b.pid, this);
            lbl->setFixedSize(60, 20);
            lbl->setAlignment(Qt::AlignCenter);
            // Sin padding ni margin dentro del label
            lbl->setStyleSheet(QString(
                "background-color: %1; color: white; "
                "font-size: 11px; border: 1px solid #666; border-radius: 3px; "
                "margin: 0; padding: 0;")
                .arg(c.name()));
            leyendaLayout->addWidget(lbl);
        }
    }
    QWidget *widgetLeyenda = new QWidget(this);
    widgetLeyenda->setLayout(leyendaLayout);
    mainLayout->addWidget(widgetLeyenda);

    // 4) ScrollArea que contendrá los bloques del Gantt
    contenedorBloques = new QWidget(this);
    // Eliminamos cualquier margen del widget contenedor
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

    // Eliminamos márgenes del QScrollArea
    scrollArea->setContentsMargins(0, 0, 0, 0);

    scrollArea->setStyleSheet(
        "QScrollArea { background-color: #fafafa; border: 1px solid #ccc; }"
    );

    mainLayout->addWidget(scrollArea, /*stretch*/ 1);

    // 5) Temporizador para animar el Gantt
    timer = new QTimer(this);
    timer->setInterval(200); // 200 ms por “tick”
    connect(timer, &QTimer::timeout, this, &GanttWindow::mostrarSiguienteBloqueUnitario);
    timer->start();
}

GanttWindow::~GanttWindow() {
    if (timer->isActive()) {
        timer->stop();
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

// Slot que se llama cada 200 ms para pintar un bloque unitario
void GanttWindow::mostrarSiguienteBloqueUnitario() {
    if (indiceBloque >= static_cast<int>(bloques.size())) {
        timer->stop();
        return;
    }

    // Tomamos el bloque lógico actual
    const BloqueGantt &b = bloques[indiceBloque];
    // Calculamos el número de ciclo correspondiente a este bloque unitario
    int cicloActual = b.inicio + unidadActual;

    // Pintar un solo bloque unitario
    agregarBloqueUnitario(b.pid, cicloActual);
    unidadActual++;

    // Si ya pintamos todos los unitarios de este bloque lógico, avanzamos
    if (unidadActual >= b.duracion) {
        unidadActual = 0;
        indiceBloque++;
    }

    // Mover scroll horizontal hasta el final para que quede visible el último bloque
    QScrollBar *hbar = scrollArea->horizontalScrollBar();
    hbar->setValue(hbar->maximum());
}

// Crea un QLabel para un solo ciclo de ejecución:
//  - Muestra el número de ciclo como texto centrado.
//  - Usa el color asociado al pid para el fondo.
//  - Ancho fijo ANCHO_BASE_UNIDAD (sin separación).
void GanttWindow::agregarBloqueUnitario(const QString &pid, int ciclo) {
    QColor colorProceso = colores.value(pid);

    // Ancho base sin márgenes
    int anchoFinal = ANCHO_BASE_UNIDAD;

    QLabel *bloqueUnitario = new QLabel(QString::number(ciclo), contenedorBloques);
    bloqueUnitario->setFixedSize(anchoFinal, ALTO_UNIDAD);
    bloqueUnitario->setAlignment(Qt::AlignCenter);
    // Sin frame ni margin/padding extra
    bloqueUnitario->setFrameShape(QFrame::NoFrame);
    bloqueUnitario->setStyleSheet(QString(
        "background-color: %1; color: white; font-size: 10px; "
        "border: 1px solid #444; margin: 0; padding: 0;")
        .arg(colorProceso.name()));
    bloqueUnitario->setToolTip(QString("%1 @ ciclo %2").arg(pid).arg(ciclo));

    layoutBloques->addWidget(bloqueUnitario);
}