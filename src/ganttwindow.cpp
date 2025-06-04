#include "ganttwindow.h"
#include <QLabel>
#include <QFrame>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QVBoxLayout>
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
void GanttWindow::agregarBloqueSync(const QString &pid,
                                    const QString &recurso,
                                    const QString &accion, // "READ" o "WRITE"
                                    int ciclo,
                                    bool accessed)
{
    // 1) Actualizar la etiqueta de ciclo
    actualizarNumeroCiclo(ciclo);

    // 2) Obtener color base y ajustar para ACCESS vs WAIT
    QColor base = colorParaPID(pid);
    QColor colorProceso;
    if (accessed) {
        // ACCESS: tono claro 
        colorProceso = QColor::fromHsv(base.hue(), 180, 180).lighter(120);
    } else {
        // WAIT: rojo oscuro
        colorProceso = QColor(200, 50, 50);
    }

    // 3) Crear un contenedor vertical que contiene:
    //       - el QLabel con “ACCESS” o “WAIT” arriba,
    //       - y debajo el QLabel de 40×40 con PID y recurso.
    QWidget *wrapper = new QWidget(contenedorBloques);

    // Creamos un QVBoxLayout dentro del wrapper
    QVBoxLayout *vbox = new QVBoxLayout(wrapper);
    // Quitamos márgenes para que no haya espacio extra alrededor
    vbox->setContentsMargins(0, 0, 0, 0);
    // Dejamos un pequeño espacio (2 px) entre el statusLabel y el bloque
    vbox->setSpacing(2);
    // alinear todo el contenido al tope arriba y centrado horizontalmente
    vbox->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    // 4) Crear el QLabel de “ACCESS” o “WAIT”
    QLabel *statusLabel = new QLabel(wrapper);
    statusLabel->setText(accessed ? "ACCESS" : "WAIT");
    {
        QFont fontStatus = statusLabel->font();
        fontStatus.setPointSize(6);
        fontStatus.setBold(true);
        statusLabel->setFont(fontStatus);
    }
    // Centro horizontal y vertical dentro de su propia área
    statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    // 5) Crear el QLabel del bloque de 40×40
    QString textoInterno = QString("%1\n%2: %3")
                              .arg(pid)                  // e.g. "P1"
                              .arg(accion.left(1))       // "R" o "W"
                              .arg(recurso);             // e.g. "R1"

    QLabel *bloqueUnitario = new QLabel(textoInterno, wrapper);
    bloqueUnitario->setFixedSize(ANCHO_BASE_UNIDAD, ALTO_UNIDAD);
    bloqueUnitario->setAlignment(Qt::AlignCenter);
    bloqueUnitario->setFrameShape(QFrame::Box);
    bloqueUnitario->setLineWidth(1);
    {
        QFont fontBloque = bloqueUnitario->font();
        fontBloque.setPointSize(9);
        fontBloque.setBold(true);
        bloqueUnitario->setFont(fontBloque);
    }
    bloqueUnitario->setStyleSheet(QString(
        "background-color: %1; color: white; "
        "border: 1px solid #444; margin: 0; padding: 0;")
        .arg(colorProceso.name()));

    // Tooltip con información completa (PID, acción, recurso, estado y ciclo)
    bloqueUnitario->setToolTip(
        QString("Proceso: %1\nAcción: %2\nRecurso: %3\nEstado: %4\nCiclo: %5")
            .arg(pid)
            .arg(accion)                          // "READ" o "WRITE"
            .arg(recurso)                         // nombre del recurso
            .arg(accessed ? "ACCESS" : "WAIT")
            .arg(ciclo)
    );

    // 6) Añadir ambos widgets al vbox
    vbox->addWidget(statusLabel);
    vbox->addWidget(bloqueUnitario);

    // 7) Ajustar el tamaño del wrapper para que quepa el statusLabel + bloque de 40×40
    // obtenemos la altura que ocupa el statusLabel:
    int altoStatus = statusLabel->sizeHint().height();
    // El espacio entre statusLabel y bloque es 2 px:
    int espacio = vbox->spacing();
    // Altura total deseada = altoStatus + espacio + ALTO_UNIDAD
    int altoTotal  = altoStatus + espacio + ALTO_UNIDAD;
    // Ancho del wrapper = ancho del bloque = ANCHO_BASE_UNIDAD
    int anchoTotal = ANCHO_BASE_UNIDAD;

    wrapper->setFixedSize(anchoTotal, altoTotal);

    // 8) Insertar el wrapper completo en el layout horizontal
    layoutBloques->addWidget(wrapper);

    // 9) Auto‐scroll horizontal al final (con un breve delay)
    QScrollBar *hbar = scrollArea->horizontalScrollBar();
    QTimer::singleShot(50, [hbar]() {
        hbar->setValue(hbar->maximum());
    });
}