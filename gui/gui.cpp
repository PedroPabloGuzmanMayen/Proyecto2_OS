#include "gui.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include "proceso.h"
#include "algoritmo.h"
#include "ganttwindow.h" 

SimuladorGUI::SimuladorGUI(QWidget *parent)
    : QMainWindow(parent) {

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // Crear widgets
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

    btnSimA = new QPushButton("Simulación A", this);
    btnSimB = new QPushButton("Simulación B", this);

    // Agregar widgets al layout
    layout->addWidget(comboAlgoritmo);
    layout->addWidget(labelQuantum);
    layout->addWidget(spinQuantum);
    layout->addWidget(btnSimA);
    layout->addWidget(btnSimB);

    // Ocultar quantum al inicio
    labelQuantum->setVisible(false);
    spinQuantum->setVisible(false);

    // Mostrar quantum si se elige Round Robin
    connect(comboAlgoritmo, &QComboBox::currentTextChanged, this, [=](const QString &text) {
        bool esRR = text.contains("Round Robin", Qt::CaseInsensitive);
        labelQuantum->setVisible(esRR);
        spinQuantum->setVisible(esRR);
    });

    // Conectar botones
    connect(btnSimA, &QPushButton::clicked, this, &SimuladorGUI::onSimulacionAClicked);
    connect(btnSimB, &QPushButton::clicked, this, &SimuladorGUI::onSimulacionBClicked);

    setCentralWidget(central);
    setWindowTitle("Simulador de Sistemas Operativos");
}

void SimuladorGUI::onSimulacionAClicked() {
    auto procesos = cargarProcesosDesdeArchivo("../data/procesos.txt");

    if (procesos.empty()) {
        QMessageBox::warning(this, "Error", "No se cargaron procesos.");
        return;
    }

    QString algoritmo = comboAlgoritmo->currentText();
    int quantum = spinQuantum->value();
    std::vector<Proceso> ejecutados;

    if (algoritmo.contains("FIFO", Qt::CaseInsensitive)) {
        ejecutados = fifo(procesos);
    } else if (algoritmo.contains("Round Robin", Qt::CaseInsensitive)) {
        ejecutados = roundRobin(procesos, quantum);
    } else {
        QMessageBox::information(this, "Info", "Ese algoritmo aún no está implementado.");
        return;
    }

    QString resultado = "Orden de ejecución:\n";
    for (const auto& p : ejecutados) {
        resultado += p.pid + "\n";
    }
    
    // Calcular tiempo de espera promedio
    double promedio = calcularTiempoEsperaPromedio(procesos, ejecutados);
    resultado += "\nTiempo de espera promedio: ";
    resultado += QString::number(promedio, 'f', 2);
    // Mostrar diagrama de Gantt
    GanttWindow *gantt = new GanttWindow(ejecutados, this);
    gantt->setAttribute(Qt::WA_DeleteOnClose);  // se elimina al cerrarse
    gantt->show();
    
    QMessageBox::information(this, "Resultado", resultado);    
}

void SimuladorGUI::onSimulacionBClicked() {
    QMessageBox::information(this, "Info", "Simulación B aún no implementada.");
}