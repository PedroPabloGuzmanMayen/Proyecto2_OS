#include "ganttwindow.h"
#include <QLabel>
#include <QFrame>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTimer>

GanttWindow::GanttWindow(QWidget *parent)
    : QWidget(parent),
      indiceBloque(0),
      unidadActual(0),
      timer(nullptr)
{
    // 1) Layout principal vertical
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(10);

    // 2) Crear y añadir la etiqueta de “Ciclo: X”
    labelCiclo = new QLabel("Ciclo: 0", this);          
    labelCiclo->setAlignment(Qt::AlignCenter);
    labelCiclo->setStyleSheet("font-weight: bold; font-size: 14px; margin: 4px;");
    mainLayout->addWidget(labelCiclo);

    // 3) Título centrado
    QLabel *titulo = new QLabel("Diagrama de Gantt", this);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-weight: bold; font-size: 18px; color: #333;");
    mainLayout->addWidget(titulo);

    // 4) Contenedor de bloques (un QWidget con QHBoxLayout)
    contenedorBloques = new QWidget(this);
    contenedorBloques->setContentsMargins(0, 0, 0, 0);

    layoutBloques = new QHBoxLayout(contenedorBloques);
    layoutBloques->setSpacing(4);               // Separación de 4 px entre bloques
    layoutBloques->setContentsMargins(0, 0, 0, 0);

    // 5) ScrollArea que mostrará los bloques horizontalmente
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidget(contenedorBloques);
    scrollArea->setContentsMargins(0, 0, 0, 0);

    // Fijamos la altura del scroll area: bloque (40 px) + margen para scrollbar (~50 px)
    scrollArea->setFixedHeight(ALTO_UNIDAD + 50);

    scrollArea->setStyleSheet(
        "QScrollArea { background-color: #fafafa; border: 1px solid #ccc; }"
    );

    mainLayout->addWidget(scrollArea, 1);

    // 6) Establecer tamaño mínimo de la ventana de Gantt
    setMinimumSize(800, 200);
}

GanttWindow::~GanttWindow() {
    // Qt limpiará automáticamente los widgets hijos
}

// Actualiza la etiqueta superior con el ciclo actual
void GanttWindow::actualizarNumeroCiclo(int ciclo) {
    if (labelCiclo) {
        labelCiclo->setText(QString("Ciclo: %1").arg(ciclo));
    }
}

// Mapear PID a un color base (aleatorio pero consistente)
QColor GanttWindow::colorParaPID(const QString &pid) {
    if (colores.contains(pid)) {
        return colores.value(pid);
    }
    int hue = QRandomGenerator::global()->bounded(360);
    QColor c = QColor::fromHsv(hue, 180, 200);
    colores.insert(pid, c);
    return c;
}

// Bloque para Simulación A (se deja tamaño 40×40, texto en dos líneas)
void GanttWindow::agregarBloqueEnTiempoReal(const QString &pid, int ciclo) {
    actualizarNumeroCiclo(ciclo);

    QColor colorProceso = colorParaPID(pid);

    QLabel *bloqueUnitario = new QLabel(QString("%1\n%2").arg(pid).arg(ciclo),
                                        contenedorBloques);
    bloqueUnitario->setFixedSize(ANCHO_BASE_UNIDAD, ALTO_UNIDAD);
    bloqueUnitario->setAlignment(Qt::AlignCenter);
    bloqueUnitario->setFrameShape(QFrame::Box);
    bloqueUnitario->setLineWidth(1);
    bloqueUnitario->setStyleSheet(QString(
        "background-color: %1; color: white; font-size: 10px; font-weight: bold; "
        "border: 1px solid #444; margin: 0; padding: 0;")
        .arg(colorProceso.name()));
    bloqueUnitario->setToolTip(QString("Proceso: %1\nCiclo: %2").arg(pid).arg(ciclo));

    layoutBloques->addWidget(bloqueUnitario);

    // Auto-scroll horizontal al final (con un breve delay)
    QScrollBar *hbar = scrollArea->horizontalScrollBar();
    QTimer::singleShot(50, [hbar]() {
        hbar->setValue(hbar->maximum());
    });
}

// Borra todos los bloques y el mapa de colores
void GanttWindow::limpiarDiagrama() {
    QLayoutItem *item;
    while ((item = layoutBloques->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    colores.clear();
    actualizarNumeroCiclo(0);
}

// Bloque para Simulación B (ACCESS vs WAIT)
void GanttWindow::agregarBloqueSync(const QString &pid, int ciclo, bool accessed) {
    // 1) Actualizar la etiqueta de ciclo
    actualizarNumeroCiclo(ciclo);

    // 2) Obtener color base y ajustar para ACCESS vs WAIT
    QColor base = colorParaPID(pid);
    QColor colorProceso;
    if (accessed) {
        // ACCESS: tono claro (verde suave)
        colorProceso = QColor::fromHsv(base.hue(), 180, 180).lighter(120);
    } else {
        // WAIT: rojo oscuro
        colorProceso = QColor(200, 50, 50);
    }

    // 3) Crear la etiqueta del bloque con PID y ciclo (dos líneas)
    QLabel *bloqueUnitario = new QLabel(QString("%1\n%2").arg(pid).arg(ciclo),
                                        contenedorBloques);
    bloqueUnitario->setFixedSize(ANCHO_BASE_UNIDAD, ALTO_UNIDAD);
    bloqueUnitario->setAlignment(Qt::AlignCenter);
    bloqueUnitario->setFrameShape(QFrame::Box);
    bloqueUnitario->setLineWidth(1);

    // Fuente tamaño 9 para que quepa dentro de 40×40 px
    QFont fontBloque = bloqueUnitario->font();
    fontBloque.setPointSize(9);
    fontBloque.setBold(true);
    bloqueUnitario->setFont(fontBloque);

    bloqueUnitario->setStyleSheet(QString(
        "background-color: %1; color: white; border: 1px solid #444; margin: 0; padding: 0;")
        .arg(colorProceso.name()));

    // Tooltip con información completa (PID, estado y ciclo)
    bloqueUnitario->setToolTip(
        QString("Proceso: %1\nEstado: %2\nCiclo: %3")
            .arg(pid)
            .arg(accessed ? "ACCESS" : "WAIT")
            .arg(ciclo)
    );

    // 4) Añadir al layout
    layoutBloques->addWidget(bloqueUnitario);

    // 5) Auto-scroll horizontal al final (con un breve delay)
    QScrollBar *hbar = scrollArea->horizontalScrollBar();
    QTimer::singleShot(50, [hbar]() {
        hbar->setValue(hbar->maximum());
    });
}