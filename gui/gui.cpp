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
    : QMainWindow(parent), ganttWidget(nullptr) {

    QWidget *central = new QWidget(this);
    layout = new QVBoxLayout(central);  // ahora es miembro

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

    layout->addWidget(comboAlgoritmo);
    layout->addWidget(labelQuantum);
    layout->addWidget(spinQuantum);
    layout->addWidget(btnSimA);
    layout->addWidget(btnSimB);

    labelQuantum->setVisible(false);
    spinQuantum->setVisible(false);

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

void SimuladorGUI::onSimulacionAClicked() {
    auto procesos = cargarProcesosDesdeArchivo("../data/procesos.txt");

    if (procesos.empty()) {
        QMessageBox::warning(this, "Error", "No se cargaron procesos.");
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