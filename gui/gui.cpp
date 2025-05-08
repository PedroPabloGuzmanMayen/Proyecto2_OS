#include "gui.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include "proceso.h"
#include "algoritmo.h"

SimuladorGUI::SimuladorGUI(QWidget *parent)
    : QMainWindow(parent) {
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    btnSimA = new QPushButton("Simulación A", this);
    btnSimB = new QPushButton("Simulación B", this);

    layout->addWidget(btnSimA);
    layout->addWidget(btnSimB);

    connect(btnSimA, &QPushButton::clicked, this, &SimuladorGUI::onSimulacionAClicked);
    connect(btnSimB, &QPushButton::clicked, this, &SimuladorGUI::onSimulacionBClicked);

    setCentralWidget(central);
    setWindowTitle("Simulador de Sistemas Operativos");
}

void SimuladorGUI::onSimulacionAClicked() {
    auto procesos = cargarProcesosDesdeArchivo("data/procesos.txt");

    if (procesos.empty()) {
        QMessageBox::warning(this, "Error", "No se cargaron procesos.");
        return;
    }

    auto ejecutados = fifo(procesos);

    QString resultado = "Orden FIFO:\n";
    for (const auto& p : ejecutados) {
        resultado += p.pid + "\n";
    }

    QMessageBox::information(this, "FIFO", resultado);
}

void SimuladorGUI::onSimulacionBClicked() {
    QMessageBox::information(this, "Info", "Simulación B aún no implementada.");
}