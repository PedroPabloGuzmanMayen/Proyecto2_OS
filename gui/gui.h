#ifndef SIMULADORGUI_H
#define SIMULADORGUI_H

#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QString>
#include "algoritmo.h"   // Para bloques de Gantt y simulación B
#include "ganttwindow.h"
#include <QRadioButton>    // Para los nuevos radio buttons
#include <QButtonGroup>    // Para agrupar los dos radio buttons

class SimuladorGUI : public QMainWindow {
    Q_OBJECT

public:
    explicit SimuladorGUI(QWidget *parent = nullptr);

private slots:
    // Simulación A
    void onSimulacionAClicked();
    // Simulación B
    void onSimulacionBClicked();

    // Archivo procesos (Simulación A)
    void onSeleccionarArchivo();
    void onArchivoDefault();

    // Archivos para Simulación B
    void onSeleccionarProcesosSync();
    void onSeleccionarRecursosSync();
    void onSeleccionarAccionesSync();
    void onArchivoDefaultSync();

    // ===== Nuevos slots para la sección B =====
    void onVerProcesosSyncClicked();
    void onVerRecursosSyncClicked();
    void onVerAccionesSyncClicked();
    void onResetSimBSClicked();

private:
    // --- Controles de Simulación A ---
    QGroupBox   *grupoArchivo;         // Para elegir archivo de procesos
    QLineEdit   *lineEditArchivo;      // Muestra la ruta
    QPushButton *btnSeleccionarArchivo;
    QPushButton *btnArchivoDefault;

    QGroupBox   *grupoAlgoritmo;       // Para elegir algoritmo y quantum
    QComboBox   *comboAlgoritmo;
    QLabel      *labelQuantum;
    QSpinBox    *spinQuantum;

    QGroupBox   *grupoSimulacion;      // Botones “Simulación A” y “Simulación B”
    QPushButton *btnSimA;
    QPushButton *btnSimB;

    // --- Controles de Simulación B ---
    QGroupBox   *grupoSync;            // Contiene 3 subgrupos para procesos, recursos y acciones
    QLineEdit   *lineEditProcesosSync; // Ruta procesos sync
    QPushButton *btnProcesosSync;      // “Buscar Procesos”
    QPushButton *btnProcesosDefault;   // “Usar Procesos por Defecto”

    QLineEdit   *lineEditRecursosSync; // Ruta recursos sync
    QPushButton *btnRecursosSync;      // “Buscar Recursos”
    QPushButton *btnRecursosDefault;   // “Usar Recursos por Defecto”

    QLineEdit   *lineEditAccionesSync; // Ruta acciones sync
    QPushButton *btnAccionesSync;      // “Buscar Acciones”
    QPushButton *btnAccionesDefault;   // “Usar Acciones por Defecto”

    // == Controles para seleccionar Modo Mécanismo ==
    QGroupBox    *grupoModoSync;       // Contenedor “Modo de sincronización”
    QRadioButton *rbMutex;             // “Mutex Locks”
    QRadioButton *rbSemaforo;          // “Semáforo”
    QButtonGroup *buttonGroupModo;     // Agrupa los dos radio buttons

    // == Botones adicionales para Simulación B ==
    QPushButton *btnVerProcesosSync;   // “Ver Procesos”
    QPushButton *btnVerRecursosSync;   // “Ver Recursos”
    QPushButton *btnVerAccionesSync;   // “Ver Acciones”
    QPushButton *btnResetSimBS;        // “Limpiar Simulación B”

    // --- Para visualización de Simulación A y B ---
    GanttWindow *ganttWidget;         // Bloques de Gantt embebidos
    QVBoxLayout *layout;              // Layout principal para reordenar dinámicamente

    // Rutas seleccionadas
    QString archivoSeleccionado;      // Para Simulación A
    QString procesosSyncRuta;         // Para Simulación B
    QString recursosSyncRuta;
    QString accionesSyncRuta;
};

#endif // SIMULADORGUI_H