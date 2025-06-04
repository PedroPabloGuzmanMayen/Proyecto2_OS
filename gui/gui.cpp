#include "gui.h"
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "algoritmo.h"
#include "synchronizer.h"
#include "ganttwindow.h"     // Necesario para usar GanttWindow
#include <QThread>           // Para QThread::msleep
#include <QApplication>      // Para processEvents()
#include <QDesktopServices>  
#include <QUrl>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>

// Constructor principal
SimuladorGUI::SimuladorGUI(QWidget *parent)
    : QMainWindow(parent),
      ganttWidget(nullptr),
      archivoSeleccionado("../data/procesos.txt"),
      procesosSyncRuta("../data/procesos.txt"),
      recursosSyncRuta("../data/recursos.txt"),
      accionesSyncRuta("../data/acciones.txt")
{
    // Widget central y layout principal
    QWidget *central = new QWidget(this);
    layout = new QVBoxLayout(central);

    // ------ Grupo: Archivo (Simulación A) ------
    grupoArchivo = new QGroupBox("Archivo de Procesos (Simulación A)", this);
    QHBoxLayout *hArchivo = new QHBoxLayout(grupoArchivo);

    lineEditArchivo = new QLineEdit(archivoSeleccionado, this);
    lineEditArchivo->setReadOnly(true);
    btnSeleccionarArchivo = new QPushButton("Buscar Archivo...", this);
    btnArchivoDefault      = new QPushButton("Usar por Defecto", this);

    hArchivo->addWidget(lineEditArchivo);
    hArchivo->addWidget(btnSeleccionarArchivo);
    hArchivo->addWidget(btnArchivoDefault);

    // Conexiones
    connect(btnSeleccionarArchivo, &QPushButton::clicked, this, &SimuladorGUI::onSeleccionarArchivo);
    connect(btnArchivoDefault, &QPushButton::clicked, this, &SimuladorGUI::onArchivoDefault);

    layout->addWidget(grupoArchivo);

    // ------ Grupo: Algoritmo (Simulación A) ------
    grupoAlgoritmo = new QGroupBox("Elegir Algoritmo (Simulación A)", this);
    QVBoxLayout *vAlg = new QVBoxLayout(grupoAlgoritmo);

    QLabel *lblAlg = new QLabel("Algoritmo:", this);
    comboAlgoritmo = new QComboBox(this);
    comboAlgoritmo->addItems({
        "First In First Out (FIFO)",
        "Shortest Job First (SJF)",
        "Shortest Remaining Time (SRT)",
        "Round Robin",
        "Priority Scheduling"
    });

    labelQuantum = new QLabel("Quantum:", this);
    spinQuantum  = new QSpinBox(this);
    spinQuantum->setRange(1, 50);
    spinQuantum->setValue(4);

    labelQuantum->setVisible(false);
    spinQuantum->setVisible(false);

    connect(comboAlgoritmo, &QComboBox::currentTextChanged, this, [=](const QString &text){
        bool esRR = text.contains("Round Robin", Qt::CaseInsensitive);
        labelQuantum->setVisible(esRR);
        spinQuantum->setVisible(esRR);
    });

    QHBoxLayout *hQuantum = new QHBoxLayout();
    hQuantum->addWidget(labelQuantum);
    hQuantum->addWidget(spinQuantum);

    vAlg->addWidget(lblAlg);
    vAlg->addWidget(comboAlgoritmo);
    vAlg->addLayout(hQuantum);

    layout->addWidget(grupoAlgoritmo);

    // ------ Grupo: Simulación (Botones A y B) ------
    grupoSimulacion = new QGroupBox("Ejecutar Simulación", this);
    QHBoxLayout *hSim = new QHBoxLayout(grupoSimulacion);

    btnSimA = new QPushButton("Simulación A", this);
    btnSimB = new QPushButton("Simulación B", this);

    hSim->addWidget(btnSimA);
    hSim->addWidget(btnSimB);

    connect(btnSimA, &QPushButton::clicked, this, &SimuladorGUI::onSimulacionAClicked);
    connect(btnSimB, &QPushButton::clicked, this, &SimuladorGUI::onSimulacionBClicked);

    layout->addWidget(grupoSimulacion);

    // Botón “Abrir resultados Simulación A” 
    btnAbrirResultadosA = new QPushButton("Abrir resultados Simulación A", this);
    layout->addWidget(btnAbrirResultadosA);
    connect(btnAbrirResultadosA, &QPushButton::clicked,
            this, &SimuladorGUI::onAbrirResultadosAClicked);

    // ------ Grupo: Simulación B (Sincronización) ------
    grupoSync = new QGroupBox("Archivos Simulación B (Mutex/Semáforo)", this);
    QVBoxLayout *vSync = new QVBoxLayout(grupoSync);

    // Línea: Procesos
    {
        QHBoxLayout *h1 = new QHBoxLayout();
        lineEditProcesosSync = new QLineEdit(procesosSyncRuta, this);
        lineEditProcesosSync->setReadOnly(true);
        btnProcesosSync = new QPushButton("Buscar Procesos...", this);
        btnProcesosDefault = new QPushButton("Default Procesos", this);

        h1->addWidget(lineEditProcesosSync);
        h1->addWidget(btnProcesosSync);
        h1->addWidget(btnProcesosDefault);

        connect(btnProcesosSync,   &QPushButton::clicked, this, &SimuladorGUI::onSeleccionarProcesosSync);
        connect(btnProcesosDefault,&QPushButton::clicked, this, &SimuladorGUI::onArchivoDefaultSync);

        vSync->addLayout(h1);
    }
    // Línea: Recursos
    {
        QHBoxLayout *h2 = new QHBoxLayout();
        lineEditRecursosSync = new QLineEdit(recursosSyncRuta, this);
        lineEditRecursosSync->setReadOnly(true);
        btnRecursosSync   = new QPushButton("Buscar Recursos...", this);
        btnRecursosDefault= new QPushButton("Default Recursos", this);

        h2->addWidget(lineEditRecursosSync);
        h2->addWidget(btnRecursosSync);
        h2->addWidget(btnRecursosDefault);

        connect(btnRecursosSync,    &QPushButton::clicked, this, &SimuladorGUI::onSeleccionarRecursosSync);
        connect(btnRecursosDefault, &QPushButton::clicked, this, &SimuladorGUI::onArchivoDefaultSync);

        vSync->addLayout(h2);
    }
    // Línea: Acciones
    {
        QHBoxLayout *h3 = new QHBoxLayout();
        lineEditAccionesSync = new QLineEdit(accionesSyncRuta, this);
        lineEditAccionesSync->setReadOnly(true);
        btnAccionesSync    = new QPushButton("Buscar Acciones...", this);
        btnAccionesDefault = new QPushButton("Default Acciones", this);

        h3->addWidget(lineEditAccionesSync);
        h3->addWidget(btnAccionesSync);
        h3->addWidget(btnAccionesDefault);

        connect(btnAccionesSync,    &QPushButton::clicked, this, &SimuladorGUI::onSeleccionarAccionesSync);
        connect(btnAccionesDefault, &QPushButton::clicked, this, &SimuladorGUI::onArchivoDefaultSync);

        vSync->addLayout(h3);
    }

    // ----- NUEVO: Grupo de Modo de Sincronización -----
    grupoModoSync = new QGroupBox("Modo de sincronización", this);
    QHBoxLayout *modoLayout = new QHBoxLayout(grupoModoSync);

    rbMutex    = new QRadioButton("Mutex Locks", grupoModoSync);
    rbSemaforo = new QRadioButton("Semáforo", grupoModoSync);
    rbMutex->setChecked(true); // Por defecto seleccionamos Mutex

    buttonGroupModo = new QButtonGroup(grupoModoSync);
    buttonGroupModo->addButton(rbMutex);
    buttonGroupModo->addButton(rbSemaforo);

    modoLayout->addWidget(rbMutex);
    modoLayout->addWidget(rbSemaforo);
    modoLayout->addStretch(); // Empujar a la izquierda

    vSync->addWidget(grupoModoSync);
    // --------------------------------------------------

    // Botones para ver procesos, recursos y acciones
    QHBoxLayout *hVer = new QHBoxLayout();
    btnVerProcesosSync  = new QPushButton("Ver Procesos",   grupoSync);
    btnVerRecursosSync  = new QPushButton("Ver Recursos",   grupoSync);
    btnVerAccionesSync  = new QPushButton("Ver Acciones",   grupoSync);
    btnResetSimBS       = new QPushButton("Limpiar Simulación B", grupoSync);

    hVer->addWidget(btnVerProcesosSync);
    hVer->addWidget(btnVerRecursosSync);
    hVer->addWidget(btnVerAccionesSync);
    hVer->addStretch();
    hVer->addWidget(btnResetSimBS);

    vSync->addLayout(hVer);

    // Conectar las señales de esos botones a sus slots:
    connect(btnVerProcesosSync,   &QPushButton::clicked, this, &SimuladorGUI::onVerProcesosSyncClicked);
    connect(btnVerRecursosSync,   &QPushButton::clicked, this, &SimuladorGUI::onVerRecursosSyncClicked);
    connect(btnVerAccionesSync,   &QPushButton::clicked, this, &SimuladorGUI::onVerAccionesSyncClicked);
    connect(btnResetSimBS,        &QPushButton::clicked, this, &SimuladorGUI::onResetSimBSClicked);

    layout->addWidget(grupoSync);

    setCentralWidget(central);
    setWindowTitle("Simulador de Sistemas Operativos");
    resize(800, 600);
}

// ---------------------
// Slots: Selección de archivos (Simulación A)
// ---------------------
void SimuladorGUI::onSeleccionarArchivo() {
    QString archivo = QFileDialog::getOpenFileName(
        this,
        "Seleccionar archivo de procesos",
        QDir::homePath(),
        "Archivos de texto (*.txt);;Todos los archivos (*)"
    );
    if (!archivo.isEmpty()) {
        archivoSeleccionado = archivo;
        lineEditArchivo->setText(archivo);
        QFileInfo info(archivo);
        QString mensaje = QString("Archivo seleccionado:\n%1\nTamaño: %2 bytes")
                         .arg(info.fileName())
                         .arg(info.size());
        QMessageBox::information(this, "Archivo Seleccionado", mensaje);
    }
}

void SimuladorGUI::onArchivoDefault() {
    archivoSeleccionado = "../data/procesos.txt";
    lineEditArchivo->setText(archivoSeleccionado);
    QMessageBox::information(this, "Archivo por Defecto",
                             "Se utilizará el archivo por defecto: ../data/procesos.txt");
}

// ---------------------
// Slots: Selección de archivos (Simulación B)
// ---------------------
void SimuladorGUI::onSeleccionarProcesosSync() {
    QString archivo = QFileDialog::getOpenFileName(
        this,
        "Seleccionar archivo de procesos (Sync)",
        QDir::homePath(),
        "Archivos de texto (*.txt);;Todos los archivos (*)"
    );
    if (!archivo.isEmpty()) {
        procesosSyncRuta = archivo;
        lineEditProcesosSync->setText(archivo);
        QFileInfo info(archivo);
        QString mensaje = QString("Archivo de procesos (Sync) seleccionado:\n%1\nTamaño: %2 bytes")
                         .arg(info.fileName())
                         .arg(info.size());
        QMessageBox::information(this, "Procesos Sync", mensaje);
    }
}

void SimuladorGUI::onSeleccionarRecursosSync() {
    QString archivo = QFileDialog::getOpenFileName(
        this,
        "Seleccionar archivo de recursos (Sync)",
        QDir::homePath(),
        "Archivos de texto (*.txt);;Todos los archivos (*)"
    );
    if (!archivo.isEmpty()) {
        recursosSyncRuta = archivo;
        lineEditRecursosSync->setText(archivo);
        QFileInfo info(archivo);
        QString mensaje = QString("Archivo de recursos (Sync) seleccionado:\n%1\nTamaño: %2 bytes")
                         .arg(info.fileName())
                         .arg(info.size());
        QMessageBox::information(this, "Recursos Sync", mensaje);
    }
}

void SimuladorGUI::onSeleccionarAccionesSync() {
    QString archivo = QFileDialog::getOpenFileName(
        this,
        "Seleccionar archivo de acciones (Sync)",
        QDir::homePath(),
        "Archivos de texto (*.txt);;Todos los archivos (*)"
    );
    if (!archivo.isEmpty()) {
        accionesSyncRuta = archivo;
        lineEditAccionesSync->setText(archivo);
        QFileInfo info(archivo);
        QString mensaje = QString("Archivo de acciones (Sync) seleccionado:\n%1\nTamaño: %2 bytes")
                         .arg(info.fileName())
                         .arg(info.size());
        QMessageBox::information(this, "Acciones Sync", mensaje);
    }
}

void SimuladorGUI::onArchivoDefaultSync() {
    procesosSyncRuta  = "../data/procesos.txt";
    recursosSyncRuta  = "../data/recursos.txt";
    accionesSyncRuta  = "../data/acciones.txt";
    lineEditProcesosSync->setText(procesosSyncRuta);
    lineEditRecursosSync->setText(recursosSyncRuta);
    lineEditAccionesSync->setText(accionesSyncRuta);

    QMessageBox::information(this, "Archivos por Defecto",
        "Se usarán los archivos por defecto:\n"
        " - Procesos: ../data/procesos.txt\n"
        " - Recursos: ../data/recursos.txt\n"
        " - Acciones: ../data/acciones.txt");
}

// ---------------------
// Slots: Mostrar contenido cargado (Simulación B)
// ---------------------
void SimuladorGUI::onVerProcesosSyncClicked() {
    auto procesos = loadProcesos(procesosSyncRuta);
    if (procesos.empty()) {
        QMessageBox::information(this, "Procesos Sync", "No hay procesos cargados.");
        return;
    }
    QString texto;
    for (const auto &p : procesos) {
        texto += QString("%1, BT=%2, AT=%3, Prio=%4\n")
                     .arg(p.pid)
                     .arg(p.burstTime)
                     .arg(p.arrivalTime)
                     .arg(p.priority);
    }
    QMessageBox msg(this);
    msg.setWindowTitle("Lista de Procesos (Sync)");
    msg.setText(texto);
    msg.exec();
}

void SimuladorGUI::onVerRecursosSyncClicked() {
    auto recursos = loadRecursos(recursosSyncRuta);
    if (recursos.empty()) {
        QMessageBox::information(this, "Recursos Sync", "No hay recursos cargados.");
        return;
    }
    QString texto;
    for (const auto &r : recursos) {
        texto += QString("%1, Contador=%2\n")
                     .arg(r.name)
                     .arg(r.count);
    }
    QMessageBox msg(this);
    msg.setWindowTitle("Lista de Recursos (Sync)");
    msg.setText(texto);
    msg.exec();
}

void SimuladorGUI::onVerAccionesSyncClicked() {
    auto acciones = loadAcciones(accionesSyncRuta);
    if (acciones.empty()) {
        QMessageBox::information(this, "Acciones Sync", "No hay acciones cargadas.");
        return;
    }
    QString texto;
    for (const auto &a : acciones) {
        QString tipoStr = (a.type == ActionType::READ) ? "READ" : "WRITE";
        texto += QString("%1 | %2 | %3 | Ciclo: %4\n")
                     .arg(a.pid)
                     .arg(tipoStr)
                     .arg(a.recurso)
                     .arg(a.cycle);
    }
    QMessageBox msg(this);
    msg.setWindowTitle("Lista de Acciones (Sync)");
    msg.setText(texto);
    msg.exec();
}

void SimuladorGUI::onResetSimBSClicked() {
    // 1) Borrar el Gantt actual (si existe)
    if (ganttWidget) {
        layout->removeWidget(ganttWidget);
        delete ganttWidget;
        ganttWidget = nullptr;
    }

    // 2) Limpiar las rutas de los QLineEdit
    procesosSyncRuta.clear();
    recursosSyncRuta.clear();
    accionesSyncRuta.clear();
    lineEditProcesosSync->clear();
    lineEditRecursosSync->clear();
    lineEditAccionesSync->clear();

    // 3) Volver a “Mutex” por defecto
    rbMutex->setChecked(true);
    rbSemaforo->setChecked(false);

    QMessageBox::information(this, "Reset Simulación B",
        "Simulación B ha sido reiniciada. Cargue nuevos archivos o use Default.");
}

// ---------------------
// Slot: Simulación A (Calendarización)
// ---------------------
void SimuladorGUI::onSimulacionAClicked() {
    // 1) Validar que exista el archivo de procesos
    if (archivoSeleccionado.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor seleccione un archivo de procesos.");
        return;
    }
    QFileInfo infoA(archivoSeleccionado);
    if (!infoA.exists()) {
        QMessageBox::warning(this, "Error",
                             QString("El archivo seleccionado no existe:\n%1").arg(archivoSeleccionado));
        return;
    }

    // 2) Leer procesos
    auto procesos = cargarProcesosDesdeArchivo(archivoSeleccionado);
    if (procesos.empty()) {
        QMessageBox::warning(this, "Error", "No se cargaron procesos o formato incorrecto.");
        return;
    }

    // 3) Determinar algoritmo y ejecutar
    QString algoritmo = comboAlgoritmo->currentText();
    int quantum = spinQuantum->value();
    // Agregar el componente de Gantt
    if (ganttWidget) {
        layout->removeWidget(ganttWidget);
        delete ganttWidget;
        ganttWidget = nullptr;
    }
    ganttWidget = new GanttWindow(this);
    layout->addWidget(ganttWidget);

    std::vector<Proceso> ejecutados;
    std::vector<BloqueGantt> bloques;

    if (algoritmo.contains("FIFO", Qt::CaseInsensitive)) {
        ejecutados = fifo(procesos, ganttWidget);
    }
    else if (algoritmo.contains("Shortest Job First", Qt::CaseInsensitive)) {
        ejecutados = shortestJobFirst(procesos, ganttWidget);
    }
    else if (algoritmo.contains("Priority Scheduling", Qt::CaseInsensitive)) {
        ejecutados = priorityScheduling(procesos, ganttWidget);
    }
    else if (algoritmo.contains("Shortest Remaining Time", Qt::CaseInsensitive)) {
        ejecutados = shortestRemainingTime(procesos, bloques, ganttWidget);
    }
    else if (algoritmo.contains("Round Robin", Qt::CaseInsensitive)) {
        ejecutados = roundRobin(procesos, quantum, bloques, ganttWidget);
    }
    else {
        QMessageBox::information(this, "Info", "Ese algoritmo aún no está implementado.");
        return;
    }

    // ESCRIBIR resultados_simA.txt
    {
        QString rutaSalida = "resultados_simA.txt";
        QFile file(rutaSalida);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            // Escribir encabezado CSV
            out << "PID,StartTime,CompletionTime,WaitingTime,TurnaroundTime\n";
            // Recorrer cada proceso ya ejecutado y escribir sus métricas
            for (const Proceso &p : ejecutados) {
                out << p.pid << ","
                    << p.startTime << ","
                    << p.completionTime << ","
                    << p.waitingTime << ","
                    << p.turnaroundTime << "\n";
            }
            file.close();
        } else {
            QMessageBox::warning(this, "Error al escribir archivo",
                                 "No se pudo crear 'resultados_simA.txt'.");
        }
    }

    // 4) Mostrar orden y tiempo promedio
    QString resultado = "Orden de ejecución:\n";
    for (const auto& p : ejecutados) {
        resultado += QString("%1 (inicio: %2)\n").arg(p.pid).arg(p.startTime);
    }
    double promedio = calcularTiempoEsperaPromedio(procesos, ejecutados);
    resultado += "\nTiempo de espera promedio: " + QString::number(promedio, 'f', 2);

    // 6) Mostrar cuadro de texto con métricas
    QMessageBox::information(this, "Resultado Simulación A", resultado);
}

// ---------------------
// Slot: Simulación B (Mutex/Semáforo)
// ---------------------
void SimuladorGUI::onSimulacionBClicked() {
    // 1) Validar que los archivos existan
    QFileInfo infoP(procesosSyncRuta), infoR(recursosSyncRuta), infoA(accionesSyncRuta);
    if (!infoP.exists() || !infoR.exists() || !infoA.exists()) {
        QMessageBox::warning(this, "Error",
            "Alguno de los archivos de Simulación B no existe. Por favor verifique:\n"
            + procesosSyncRuta + "\n" + recursosSyncRuta + "\n" + accionesSyncRuta);
        return;
    }

    // 2) Cargar los datos
    auto procSync = loadProcesos(procesosSyncRuta);
    auto recsSync = loadRecursos(recursosSyncRuta);
    auto actsSync = loadAcciones(accionesSyncRuta);

    if (procSync.empty()) {
        QMessageBox::warning(this, "Error", "No se cargaron procesos para Simulación B.");
        return;
    }
    if (recsSync.empty()) {
        QMessageBox::warning(this, "Error", "No se cargaron recursos para Simulación B.");
        return;
    }
    if (actsSync.empty()) {
        QMessageBox::warning(this, "Error", "No se cargaron acciones para Simulación B.");
        return;
    }

    // 3) Revisar el modo seleccionado (Mutex vs Semáforo)
    bool usarSemaforo = rbSemaforo->isChecked();
    std::vector<BloqueSync> timeline;
    if (!usarSemaforo) {
        // Modo Mutex (contador implícito = 1 por recurso)
        timeline = simulateMutex(actsSync, recsSync, procSync);
    } else {
        // Modo Semáforo (contador > 1 en recursos)
        // Supón que tienes otra función simulateSyncSemaforo(...) que maneje semáforos.
        timeline = simulateSyncSemaforo(actsSync, recsSync, procSync);
    }

    // 4) CREAR (o reemplazar) el GanttWindow para mostrar los bloques coloreados
    if (ganttWidget) {
        layout->removeWidget(ganttWidget);
        delete ganttWidget;
        ganttWidget = nullptr;
    }
    ganttWidget = new GanttWindow(this);
    layout->addWidget(ganttWidget);

    // 5) Pintar cada bloque en el GanttWindow pasando PID, Recurso y Acción (verde para ACCESS, rojo para WAIT)
    for (const BloqueSync &b : timeline) {
        int inicio   = b.start;
        int duracion = b.duration;
        for (int offset = 0; offset < duracion; ++offset) {
            int cicloActual = inicio + offset;
            // Llamamos a la nueva firma: (pid, recurso, accion, ciclo, accessed)
            static_cast<GanttWindow*>(ganttWidget)
                ->agregarBloqueSync(
                    b.pid,          // nombre del proceso
                    b.recurso,      // recurso sobre el que actúa
                    b.accion,       // "READ" o "WRITE"
                    cicloActual,    // ciclo actual (start + offset)
                    b.accessed      // true = ACCESS, false = WAIT
                );
            QThread::msleep(200);           // breve retardo para la animación
            QApplication::processEvents();  // refresco de la GUI
        }
    }

    // 6) Construir el HTML para el cuadro de diálogo (opcional)
    QString resultadoHtml;
    resultadoHtml.reserve(timeline.size() * 50);
    for (const auto &b : timeline) {
        QString estadoColor = b.accessed
            ? "<span style='color:green; font-weight:bold;'>ACCESS</span>"
            : "<span style='color:red; font-weight:bold;'>WAIT</span>";
        resultadoHtml += QString("%1 | %2 | Ciclo: %3 | %4<br>")
                             .arg(b.pid)
                             .arg(b.recurso)
                             .arg(b.start)
                             .arg(estadoColor);
    }
    if (resultadoHtml.isEmpty()) {
        resultadoHtml = "No se generó ningún bloque para Simulación B.";
    }

    // 7) Mostrar un QMessageBox con ese HTML (opcional)
    QMessageBox msg(this);
    msg.setWindowTitle("Resultado Simulación B");
    msg.setTextFormat(Qt::RichText);
    msg.setText(resultadoHtml);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

void SimuladorGUI::onAbrirResultadosAClicked() {
    // 1) Ruta absoluta dentro de build/
    QString rutaSalida = QDir::current().absoluteFilePath("resultados_simA.txt");

    // 2) Verificar que exista el archivo
    if (!QFile::exists(rutaSalida)) {
        QMessageBox::warning(this, "Archivo no encontrado",
                             "No existe 'resultados_simA.txt'. Ejecuta primero Simulación A.");
        return;
    }

    // 3) Leer el contenido completo del archivo
    QFile file(rutaSalida);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error al leer archivo",
                             QString("No se pudo abrir '%1' para lectura.").arg(rutaSalida));
        return;
    }
    QTextStream in(&file);
    QString contenido = in.readAll();
    file.close();

    // 4) Crear un diálogo modal con QTextEdit para mostrarlo
    QDialog dialog(this);
    dialog.setWindowTitle("Resultados Simulación A");
    dialog.resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QTextEdit *textEdit = new QTextEdit(&dialog);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(contenido);
    layout->addWidget(textEdit);

    dialog.exec();
}