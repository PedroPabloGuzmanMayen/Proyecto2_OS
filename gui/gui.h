#ifndef SIMULADORGUI_H
#define SIMULADORGUI_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGroupBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>

#include "ganttwindow.h"

class SimuladorGUI : public QMainWindow {
    Q_OBJECT

public:
    SimuladorGUI(QWidget *parent = nullptr);

private slots:
    void onSeleccionarArchivo();
    void onArchivoDefault();
    void onSimulacionAClicked();
    void onSimulacionBClicked();

private:
    QPushButton *btnSimA;
    QPushButton *btnSimB;
    QComboBox *comboAlgoritmo;
    QSpinBox *spinQuantum;
    QLabel *labelQuantum;

    GanttWindow *ganttWidget;      // Diagrama de Gantt actual
    QVBoxLayout *layout;           // Layout principal para poder modificarlo dinámicamente
    QLineEdit *lineEditArchivo;
    QPushButton *btnSeleccionarArchivo;
    QPushButton *btnArchivoDefault;
    QString archivoSeleccionado;
};

#endif