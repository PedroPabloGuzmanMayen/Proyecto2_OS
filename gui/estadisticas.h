#ifndef ESTADISTICAS_H
#define ESTADISTICAS_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <vector>
#include "algoritmo.h"

struct ResultadoAlgoritmo {
    QString nombre;
    double tiempoEsperaPromedio;
    std::vector<Proceso> procesosEjecutados;
    bool ejecutado;
    
    ResultadoAlgoritmo() : tiempoEsperaPromedio(0.0), ejecutado(false) {}
};

class EstadisticasWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit EstadisticasWindow(QWidget *parent = nullptr);

private slots:
    void onSeleccionarArchivo();
    void onArchivoDefault();
    void onCalcularEstadisticas();
    void onRoundRobinToggled(bool checked);

private:
    // Controles de selección de archivo
    QGroupBox *grupoArchivo;
    QLineEdit *lineEditArchivo;
    QPushButton *btnSeleccionarArchivo;
    QPushButton *btnArchivoDefault;
    QString archivoSeleccionado;

    // Controles de selección de algoritmos
    QGroupBox *grupoAlgoritmos;
    QCheckBox *checkFIFO;
    QCheckBox *checkRoundRobin;
    QCheckBox *checkSJF;
    QCheckBox *checkPriority;
    QCheckBox *checkSRT;
    
    // Control de quantum para Round Robin
    QLabel *labelQuantum;
    QSpinBox *spinQuantum;
    
    // Botón de cálculo
    QPushButton *btnCalcular;
    
    // Tabla de resultados
    QGroupBox *grupoResultados;
    QTableWidget *tablaResultados;
    QLabel *labelMejorAlgoritmo;
    
    // Layout principal
    QVBoxLayout *layoutPrincipal;
    
    // Métodos auxiliares
    void configurarInterfaz();
    void configurarTablaResultados();
    void actualizarTablaResultados(const std::vector<ResultadoAlgoritmo>& resultados);
    void mostrarMejorAlgoritmo(const std::vector<ResultadoAlgoritmo>& resultados);
    std::vector<ResultadoAlgoritmo> ejecutarAlgoritmos(const std::vector<Proceso>& procesos);
};

#endif // ESTADISTICAS_H