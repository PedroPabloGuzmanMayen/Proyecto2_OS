#include "mainmenu.h"
#include "estadisticas.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QFont>
#include <QMessageBox>

MainMenu::MainMenu(QWidget *parent)
    : QMainWindow(parent),
      simuladorWindow(nullptr),
      estadisticasWindow(nullptr)
{
    // Widget central y layout principal
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    
    // Título principal
    lblTitulo = new QLabel("Menú Principal", this);
    QFont titleFont = lblTitulo->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    lblTitulo->setFont(titleFont);
    lblTitulo->setAlignment(Qt::AlignCenter);
    lblTitulo->setStyleSheet("QLabel { margin: 20px; }");
    
    // Crear los botones
    btnSimuladorVisual = new QPushButton("Simulador Visual", this);
    btnCalcularEstadisticas = new QPushButton("Calcular Estadísticas de Múltiples Algoritmos", this);
    
    // Configurar estilo de los botones
    QString buttonStyle = 
        "QPushButton {"
        "    padding: 15px 30px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    border: 2px solid #3498db;"
        "    border-radius: 8px;"
        "    background-color: #3498db;"
        "    color: white;"
        "    min-width: 300px;"
        "    min-height: 50px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "    border-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}";
        
    btnSimuladorVisual->setStyleSheet(buttonStyle);
    btnCalcularEstadisticas->setStyleSheet(buttonStyle);
    
    // Conectar signals a slots
    connect(btnSimuladorVisual, &QPushButton::clicked, this, &MainMenu::onSimuladorVisualClicked);
    connect(btnCalcularEstadisticas, &QPushButton::clicked, this, &MainMenu::onCalcularEstadisticasClicked);
    
    // Layout para los botones
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(20);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->addWidget(btnSimuladorVisual);
    buttonLayout->addWidget(btnCalcularEstadisticas);
    
    // Agregar espaciado vertical
    mainLayout->addStretch(1);
    mainLayout->addWidget(lblTitulo);
    mainLayout->addStretch(1);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(2);
    
    // Configurar ventana principal
    setCentralWidget(central);
    setWindowTitle("Sistema de Simulación de Sistemas Operativos");
    resize(600, 400);
    
    // Centrar la ventana en la pantalla
    setFixedSize(600, 400);
}

void MainMenu::onSimuladorVisualClicked()
{
    // Crear y mostrar la ventana del simulador visual
    if (!simuladorWindow) {
        simuladorWindow = new SimuladorGUI();
    }
    
    simuladorWindow->show();
    simuladorWindow->raise();
    simuladorWindow->activateWindow();
    
}


void MainMenu::onCalcularEstadisticasClicked()
{
    // Crear y mostrar la ventana de estadísticas
    if (!estadisticasWindow) {
        estadisticasWindow = new EstadisticasWindow();
    }
    
    estadisticasWindow->show();
    estadisticasWindow->raise();
    estadisticasWindow->activateWindow();
}