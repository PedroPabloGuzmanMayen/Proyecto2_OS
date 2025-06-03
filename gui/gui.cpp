#include "gui.h"
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "algoritmo.h"
#include "synchronizer.h"

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

    // ------ Grupo: Simulación B (Sincronización) ------
    grupoSync = new QGroupBox("Archivos Simulación B (Mutex/Semáforo)", this);
    QVBoxLayout *vSync = new QVBoxLayout(grupoSync);

    // Linea: Procesos
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
    // Linea: Recursos
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
    // Linea: Acciones
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

    layout->addWidget(grupoSync);

    // *** Texto de ayuda o espacio para resultados ***
    // (Podrías agregar aquí un QTextEdit si quieres ver la salida de Simulación B dentro de la GUI.
    //  Por simplicidad, notificaremos resultados con QMessageBox.)

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
    //Agregar el componente de Gannt
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
    // 1) Validar archivos de Simulación B
    QFileInfo infoP(procesosSyncRuta), infoR(recursosSyncRuta), infoA(accionesSyncRuta);
    if (!infoP.exists() || !infoR.exists() || !infoA.exists()) {
        QMessageBox::warning(this, "Error",
            "Alguno de los archivos de Simulación B no existe. Por favor verifique:\n" +
            procesosSyncRuta + "\n" +
            recursosSyncRuta + "\n" +
            accionesSyncRuta);
        return;
    }

    // 2) Cargar datos
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

    // 3) Ejecutar simulación B
    std::vector<BloqueSync> timeline = simulateSync(actsSync, recsSync);

    // 4) Mostrar resultado de Simulación B
    // Construimos un string con todos los bloques: PID, recurso, ciclo y ACCESS/WAIT
    QString resultado;
    for (auto &b : timeline) {
        // Creamos el texto coloreado según b.accessed
        QString estado = b.accessed
            ? "<span style='color:green;'>ACCESS</span>"
            : "<span style='color:red;'>WAIT</span>";

        // Usamos <br> para salto de línea en texto enriquecido
        resultado += QString("%1 | %2 | Ciclo: %3 | %4<br>")
                        .arg(b.pid)
                        .arg(b.recurso)
                        .arg(b.start)
                        .arg(estado);
    }

    if (resultado.isEmpty()) {
        resultado = "No se generó ningún bloque para Simulación B.";
    }

    QMessageBox msg(this);
    msg.setWindowTitle("Resultado Simulación B");
    // Indicamos que vamos a usar texto con formato HTML
    msg.setTextFormat(Qt::RichText);
    msg.setText(resultado);
    msg.exec();
}