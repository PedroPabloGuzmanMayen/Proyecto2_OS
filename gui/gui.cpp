#include "gui.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QFileDialog>
#include <QLineEdit>
#include <QGroupBox>
#include <QFileInfo>
#include <QDir>
#include "proceso.h"
#include "algoritmo.h"
#include "ganttwindow.h" 

SimuladorGUI::SimuladorGUI(QWidget *parent)
    : QMainWindow(parent), ganttWidget(nullptr) {

    QWidget *central = new QWidget(this);
    layout = new QVBoxLayout(central);  // ahora es miembro

    // Grupo para selección de archivo
    QGroupBox *grupoArchivo = new QGroupBox("Selección de Archivo", this);
    QHBoxLayout *layoutArchivo = new QHBoxLayout(grupoArchivo);
    
    lineEditArchivo = new QLineEdit(this);
    lineEditArchivo->setPlaceholderText("Seleccione un archivo de procesos...");
    lineEditArchivo->setReadOnly(true);
    
    btnSeleccionarArchivo = new QPushButton("Buscar Archivo", this);
    btnArchivoDefault = new QPushButton("Usar Archivo por Defecto", this);
    
    layoutArchivo->addWidget(lineEditArchivo);
    layoutArchivo->addWidget(btnSeleccionarArchivo);
    layoutArchivo->addWidget(btnArchivoDefault);

    // Grupo para configuración de algoritmo
    QGroupBox *grupoAlgoritmo = new QGroupBox("Configuración del Algoritmo", this);
    QVBoxLayout *layoutAlgoritmo = new QVBoxLayout(grupoAlgoritmo);

    comboAlgoritmo = new QComboBox(this);
    comboAlgoritmo->addItems({
        "First In First Out (FIFO)",
        "Shortest Job First (SJF)",
        "Shortest Remaining Time (SRT)",
        "Round Robin",
        "Priority Scheduling"
    });

    labelQuantum = new QLabel("Quantum:", this);
    spinQuantum = new QSpinBox(this);
    spinQuantum->setRange(1, 50);
    spinQuantum->setValue(4);

    layoutAlgoritmo->addWidget(new QLabel("Algoritmo:", this));
    layoutAlgoritmo->addWidget(comboAlgoritmo);
    layoutAlgoritmo->addWidget(labelQuantum);
    layoutAlgoritmo->addWidget(spinQuantum);

    // Grupo para botones de simulación
    QGroupBox *grupoSimulacion = new QGroupBox("Simulaciones", this);
    QHBoxLayout *layoutSimulacion = new QHBoxLayout(grupoSimulacion);

    btnSimA = new QPushButton("Simulación A", this);
    btnSimB = new QPushButton("Simulación B", this);

    layoutSimulacion->addWidget(btnSimA);
    layoutSimulacion->addWidget(btnSimB);

    // Agregar todos los grupos al layout principal
    layout->addWidget(grupoArchivo);
    layout->addWidget(grupoAlgoritmo);
    layout->addWidget(grupoSimulacion);

    // Configuración inicial
    labelQuantum->setVisible(false);
    spinQuantum->setVisible(false);
    archivoSeleccionado = "../data/procesos.txt"; // Archivo por defecto
    lineEditArchivo->setText("../data/procesos.txt");

    // Conexiones
    connect(btnSeleccionarArchivo, &QPushButton::clicked, this, &SimuladorGUI::onSeleccionarArchivo);
    connect(btnArchivoDefault, &QPushButton::clicked, this, &SimuladorGUI::onArchivoDefault);

    connect(comboAlgoritmo, &QComboBox::currentTextChanged, this, [=](const QString &text) {
        bool esRR = text.contains("Round Robin", Qt::CaseInsensitive);
        labelQuantum->setVisible(esRR);
        spinQuantum->setVisible(esRR);
    });

    connect(btnSimA, &QPushButton::clicked, this, &SimuladorGUI::onSimulacionAClicked);
    connect(btnSimB, &QPushButton::clicked, this, &SimuladorGUI::onSimulacionBClicked);

    setCentralWidget(central);
    setWindowTitle("Simulador de Sistemas Operativos");
}

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
        
        // Mostrar información del archivo seleccionado
        QFileInfo info(archivo);
        QString mensaje = QString("Archivo seleccionado:\n%1\nTamaño: %2 bytes")
                         .arg(info.fileName())
                         .arg(info.size());
        QMessageBox::information(this, "Archivo Seleccionado", mensaje);
    }
}

void SimuladorGUI::onArchivoDefault() {
    archivoSeleccionado = "../data/procesos.txt";
    lineEditArchivo->setText("../data/procesos.txt");
    QMessageBox::information(this, "Archivo por Defecto", 
                           "Se utilizará el archivo por defecto: ../data/procesos.txt");
}

void SimuladorGUI::onSimulacionAClicked() {
    // Verificar que hay un archivo seleccionado
    if (archivoSeleccionado.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor seleccione un archivo de procesos.");
        return;
    }

    // Verificar que el archivo existe
    QFileInfo info(archivoSeleccionado);
    if (!info.exists()) {
        QMessageBox::warning(this, "Error", 
                           QString("El archivo seleccionado no existe:\n%1").arg(archivoSeleccionado));
        return;
    }

    auto procesos = cargarProcesosDesdeArchivo(archivoSeleccionado);

    if (procesos.empty()) {
        QMessageBox::warning(this, "Error", 
                           "No se pudieron cargar procesos del archivo seleccionado.\n"
                           "Verifique que el formato sea correcto.");
        return;
    }


    QString algoritmo = comboAlgoritmo->currentText();
    int quantum = spinQuantum->value();

    std::vector<Proceso> ejecutados;
    std::vector<BloqueGantt> bloques;

    // Selección de algoritmo:
    if (algoritmo.contains("FIFO", Qt::CaseInsensitive)) {
        ejecutados = fifo(procesos);
    }
    else if (algoritmo.contains("Shortest Job First", Qt::CaseInsensitive)) {
        ejecutados = shortestJobFirst(procesos);
    }
    else if (algoritmo.contains("Priority Scheduling", Qt::CaseInsensitive)) {
        ejecutados = priorityScheduling(procesos);
    }
    else if (algoritmo.contains("Round Robin", Qt::CaseInsensitive)) {
        ejecutados = roundRobin(procesos, quantum, bloques);
        // roundRobin ya llenó 'bloques'
    }
    else {
        QMessageBox::information(this, "Info", "Ese algoritmo aún no está implementado.");
        return;
    }

    // Construcción genérica de bloques **solo si siguen vacíos**:
    if (bloques.empty()) {
        for (const auto &p : ejecutados) {
            bloques.push_back({ p.pid, p.startTime, p.burstTime });
        }
    }

    QString resultado = "Orden de ejecución:\n";
    for (const auto& p : ejecutados) {
        resultado += QString("%1 (inicio: %2)\n").arg(p.pid).arg(p.startTime);
    }

    double promedio = calcularTiempoEsperaPromedio(procesos, ejecutados);
    resultado += "\nTiempo de espera promedio: " + QString::number(promedio, 'f', 2);

    // Mostrar Gantt embebido
    if (ganttWidget != nullptr) {
        layout->removeWidget(ganttWidget);
        delete ganttWidget;
        ganttWidget = nullptr;
    }

    ganttWidget = new GanttWindow(bloques, this);
    layout->addWidget(ganttWidget);

    QMessageBox::information(this, "Resultado", resultado);
}

void SimuladorGUI::onSimulacionBClicked() {
    QMessageBox::information(this, "Info", "Simulación B aún no implementada.");
}