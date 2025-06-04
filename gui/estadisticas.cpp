#include "estadisticas.h"
#include <QDir>
#include <QFont>
#include <algorithm>
#include <limits>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>

EstadisticasWindow::EstadisticasWindow(QWidget *parent)
    : QMainWindow(parent),
      archivoSeleccionado("../data/procesos.txt")
{
    configurarInterfaz();
    setWindowTitle("Estadísticas de Algoritmos de Calendarización");
    resize(800, 700);
}

void EstadisticasWindow::configurarInterfaz()
{
    QWidget *central = new QWidget(this);
    layoutPrincipal = new QVBoxLayout(central);
    
    // --- Grupo: Selección de Archivo ---
    grupoArchivo = new QGroupBox("Archivo de Procesos", this);
    QHBoxLayout *layoutArchivo = new QHBoxLayout(grupoArchivo);
    
    lineEditArchivo = new QLineEdit(archivoSeleccionado, this);
    lineEditArchivo->setReadOnly(true);
    btnSeleccionarArchivo = new QPushButton("Buscar Archivo...", this);
    btnArchivoDefault = new QPushButton("Usar por Defecto", this);
    
    layoutArchivo->addWidget(lineEditArchivo);
    layoutArchivo->addWidget(btnSeleccionarArchivo);
    layoutArchivo->addWidget(btnArchivoDefault);
    
    connect(btnSeleccionarArchivo, &QPushButton::clicked, this, &EstadisticasWindow::onSeleccionarArchivo);
    connect(btnArchivoDefault, &QPushButton::clicked, this, &EstadisticasWindow::onArchivoDefault);
    
    layoutPrincipal->addWidget(grupoArchivo);
    
    // --- Grupo: Selección de Algoritmos ---
    grupoAlgoritmos = new QGroupBox("Seleccionar Algoritmos a Comparar", this);
    QVBoxLayout *layoutAlgoritmos = new QVBoxLayout(grupoAlgoritmos);
    
    checkFIFO = new QCheckBox("First In First Out (FIFO)", this);
    checkRoundRobin = new QCheckBox("Round Robin", this);
    checkSJF = new QCheckBox("Shortest Job First (SJF)", this);
    checkPriority = new QCheckBox("Priority Scheduling", this);
    checkSRT = new QCheckBox("Shortest Remaining Time (SRT)", this);
    
    // Por defecto, seleccionar algunos algoritmos
    checkFIFO->setChecked(true);
    checkSJF->setChecked(true);
    
    layoutAlgoritmos->addWidget(checkFIFO);
    layoutAlgoritmos->addWidget(checkRoundRobin);
    layoutAlgoritmos->addWidget(checkSJF);
    layoutAlgoritmos->addWidget(checkPriority);
    layoutAlgoritmos->addWidget(checkSRT);
    
    // Control de Quantum para Round Robin
    QHBoxLayout *layoutQuantum = new QHBoxLayout();
    labelQuantum = new QLabel("Quantum:", this);
    spinQuantum = new QSpinBox(this);
    spinQuantum->setRange(1, 50);
    spinQuantum->setValue(4);
    
    // Inicialmente oculto
    labelQuantum->setVisible(false);
    spinQuantum->setVisible(false);
    
    layoutQuantum->addWidget(labelQuantum);
    layoutQuantum->addWidget(spinQuantum);
    layoutQuantum->addStretch();
    
    layoutAlgoritmos->addLayout(layoutQuantum);
    
    connect(checkRoundRobin, &QCheckBox::toggled, this, &EstadisticasWindow::onRoundRobinToggled);
    
    layoutPrincipal->addWidget(grupoAlgoritmos);
    
    // --- Botón de Cálculo ---
    btnCalcular = new QPushButton("Calcular Estadísticas", this);
    btnCalcular->setStyleSheet(
        "QPushButton {"
        "    padding: 12px 24px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    border: 2px solid #27ae60;"
        "    border-radius: 6px;"
        "    background-color: #27ae60;"
        "    color: white;"
        "    min-height: 40px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #229954;"
        "    border-color: #229954;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1e8449;"
        "}"
    );
    
    connect(btnCalcular, &QPushButton::clicked, this, &EstadisticasWindow::onCalcularEstadisticas);
    
    layoutPrincipal->addWidget(btnCalcular);

    // boton para abrir resultados Estadísticas
    btnAbrirResultadosEstadisticas = new QPushButton("Abrir resultados Estadísticas", this);
    layoutPrincipal->addWidget(btnAbrirResultadosEstadisticas);
    connect(btnAbrirResultadosEstadisticas, &QPushButton::clicked,
            this, &EstadisticasWindow::onAbrirResultadosEstadisticasClicked);
    
    // --- Grupo: Resultados ---
    grupoResultados = new QGroupBox("Resultados Comparativos", this);
    QVBoxLayout *layoutResultados = new QVBoxLayout(grupoResultados);
    
    configurarTablaResultados();
    layoutResultados->addWidget(tablaResultados);
    
    // Label para mostrar el mejor algoritmo
    labelMejorAlgoritmo = new QLabel("", this);
    QFont fontMejor = labelMejorAlgoritmo->font();
    fontMejor.setPointSize(12);
    fontMejor.setBold(true);
    labelMejorAlgoritmo->setFont(fontMejor);
    labelMejorAlgoritmo->setStyleSheet("QLabel { color: #27ae60; padding: 10px; }");
    labelMejorAlgoritmo->setAlignment(Qt::AlignCenter);
    
    layoutResultados->addWidget(labelMejorAlgoritmo);
    
    layoutPrincipal->addWidget(grupoResultados);
    
    setCentralWidget(central);
}

void EstadisticasWindow::configurarTablaResultados()
{
    tablaResultados = new QTableWidget(0, 3, this);
    
    // Configurar headers
    QStringList headers;
    headers << "Algoritmo" << "Tiempo de Espera Promedio" << "Estado";
    tablaResultados->setHorizontalHeaderLabels(headers);
    
    // Configurar tamaños de columnas
    tablaResultados->horizontalHeader()->setStretchLastSection(true);
    tablaResultados->setColumnWidth(0, 250);
    tablaResultados->setColumnWidth(1, 200);
    
    // Configurar propiedades de la tabla
    tablaResultados->setAlternatingRowColors(true);
    tablaResultados->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaResultados->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaResultados->verticalHeader()->setVisible(false);
}

void EstadisticasWindow::onSeleccionarArchivo()
{
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

void EstadisticasWindow::onArchivoDefault()
{
    archivoSeleccionado = "../data/procesos.txt";
    lineEditArchivo->setText(archivoSeleccionado);
    QMessageBox::information(this, "Archivo por Defecto",
                           "Se utilizará el archivo por defecto: ../data/procesos.txt");
}

void EstadisticasWindow::onRoundRobinToggled(bool checked)
{
    labelQuantum->setVisible(checked);
    spinQuantum->setVisible(checked);
}

void EstadisticasWindow::onCalcularEstadisticas()
{
    // Validar que hay al menos un algoritmo seleccionado
    if (!checkFIFO->isChecked() && !checkRoundRobin->isChecked() && 
        !checkSJF->isChecked() && !checkPriority->isChecked() && !checkSRT->isChecked()) {
        QMessageBox::warning(this, "Error", "Por favor seleccione al menos un algoritmo.");
        return;
    }
    
    // Validar que existe el archivo
    QFileInfo info(archivoSeleccionado);
    if (!info.exists()) {
        QMessageBox::warning(this, "Error",
                           QString("El archivo seleccionado no existe:\n%1").arg(archivoSeleccionado));
        return;
    }
    
    // Cargar procesos
    auto procesos = cargarProcesosDesdeArchivo(archivoSeleccionado);
    if (procesos.empty()) {
        QMessageBox::warning(this, "Error", "No se cargaron procesos o formato incorrecto.");
        return;
    }
    
    // Ejecutar algoritmos seleccionados
    std::vector<ResultadoAlgoritmo> resultados = ejecutarAlgoritmos(procesos);
    
    // Actualizar tabla con resultados
    actualizarTablaResultados(resultados);
    
    // Mostrar mejor algoritmo
    mostrarMejorAlgoritmo(resultados);
    
    // resultados_estadisticas.txt
    {
        QString rutaSalida = "resultados_estadisticas.txt";
        QFile file(rutaSalida);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            // Encabezado CSV
            out << "Algoritmo,TiempoEsperaPromedio,Estado\n";
            // Volcar cada resultado de algoritmo
            for (const ResultadoAlgoritmo &r : resultados) {
                out << r.nombre << ","
                    << r.tiempoEsperaPromedio << ","
                    << (r.ejecutado ? "Completado" : "Error") << "\n";
            }
            // Opcional: detalle por proceso en cada algoritmo
            out << "\n--- Detalle por algoritmo ---\n";
            for (const ResultadoAlgoritmo &r : resultados) {
                if (!r.ejecutado) continue;
                out << "\n" << r.nombre << "\n";
                out << "PID,Start,Completion,Waiting,Turnaround\n";
                for (const Proceso &p : r.procesosEjecutados) {
                    out << p.pid << ","
                        << p.startTime << ","
                        << p.completionTime << ","
                        << p.waitingTime << ","
                        << p.turnaroundTime << "\n";
                }
            }
            file.close();
        } else {
            QMessageBox::warning(this, "Error al escribir archivo",
                                 "No se pudo crear 'resultados_estadisticas.txt'.");
        }
    }
}

std::vector<ResultadoAlgoritmo> EstadisticasWindow::ejecutarAlgoritmos(const std::vector<Proceso>& procesos)
{
    std::vector<ResultadoAlgoritmo> resultados;
    
    // FIFO
    if (checkFIFO->isChecked()) {
        ResultadoAlgoritmo resultado;
        resultado.nombre = "First In First Out (FIFO)";
        try {
            auto procesosOriginal = procesos; // Copia para mantener el original
            resultado.procesosEjecutados = fifo(procesosOriginal);
            resultado.tiempoEsperaPromedio = calcularTiempoEsperaPromedio(procesos, resultado.procesosEjecutados);
            resultado.ejecutado = true;
        } catch (...) {
            resultado.ejecutado = false;
        }
        resultados.push_back(resultado);
    }
    
    // Round Robin
    if (checkRoundRobin->isChecked()) {
        ResultadoAlgoritmo resultado;
        resultado.nombre = QString("Round Robin (Q=%1)").arg(spinQuantum->value());
        try {
            auto procesosOriginal = procesos;
            std::vector<BloqueGantt> bloques; // Para Round Robin
            resultado.procesosEjecutados = roundRobin(procesosOriginal, spinQuantum->value(), bloques);
            resultado.tiempoEsperaPromedio = calcularTiempoEsperaPromedio(procesos, resultado.procesosEjecutados);
            resultado.ejecutado = true;
        } catch (...) {
            resultado.ejecutado = false;
        }
        resultados.push_back(resultado);
    }
    
    // Shortest Job First
    if (checkSJF->isChecked()) {
        ResultadoAlgoritmo resultado;
        resultado.nombre = "Shortest Job First (SJF)";
        try {
            auto procesosOriginal = procesos;
            resultado.procesosEjecutados = shortestJobFirst(procesosOriginal);
            resultado.tiempoEsperaPromedio = calcularTiempoEsperaPromedio(procesos, resultado.procesosEjecutados);
            resultado.ejecutado = true;
        } catch (...) {
            resultado.ejecutado = false;
        }
        resultados.push_back(resultado);
    }
    
    // Priority Scheduling
    if (checkPriority->isChecked()) {
        ResultadoAlgoritmo resultado;
        resultado.nombre = "Priority Scheduling";
        try {
            auto procesosOriginal = procesos;
            resultado.procesosEjecutados = priorityScheduling(procesosOriginal);
            resultado.tiempoEsperaPromedio = calcularTiempoEsperaPromedio(procesos, resultado.procesosEjecutados);
            resultado.ejecutado = true;
        } catch (...) {
            resultado.ejecutado = false;
        }
        resultados.push_back(resultado);
    }
    
    // Shortest Remaining Time
    if (checkSRT->isChecked()) {
        ResultadoAlgoritmo resultado;
        resultado.nombre = "Shortest Remaining Time (SRT)";
        try {
            auto procesosOriginal = procesos;
            std::vector<BloqueGantt> bloques; // Para SRT
            resultado.procesosEjecutados = shortestRemainingTime(procesosOriginal, bloques);
            resultado.tiempoEsperaPromedio = calcularTiempoEsperaPromedio(procesos, resultado.procesosEjecutados);
            resultado.ejecutado = true;
        } catch (...) {
            resultado.ejecutado = false;
        }
        resultados.push_back(resultado);
    }
    
    return resultados;
}

void EstadisticasWindow::actualizarTablaResultados(const std::vector<ResultadoAlgoritmo>& resultados)
{
    tablaResultados->setRowCount(resultados.size());
    
    for (int i = 0; i < resultados.size(); ++i) {
        const auto& resultado = resultados[i];
        
        // Columna: Algoritmo
        QTableWidgetItem *itemAlgoritmo = new QTableWidgetItem(resultado.nombre);
        tablaResultados->setItem(i, 0, itemAlgoritmo);
        
        // Columna: Tiempo de Espera Promedio
        QString tiempoTexto;
        if (resultado.ejecutado) {
            tiempoTexto = QString::number(resultado.tiempoEsperaPromedio, 'f', 2);
        } else {
            tiempoTexto = "Error";
        }
        QTableWidgetItem *itemTiempo = new QTableWidgetItem(tiempoTexto);
        if (!resultado.ejecutado) {
            itemTiempo->setBackground(QColor(255, 200, 200)); 
        }
        tablaResultados->setItem(i, 1, itemTiempo);
        
        // Columna: Estado
        QString estado = resultado.ejecutado ? "Completado" : "Error";
        QTableWidgetItem *itemEstado = new QTableWidgetItem(estado);
        if (resultado.ejecutado) {
            itemEstado->setBackground(QColor(200, 255, 200)); 
        } else {
            itemEstado->setBackground(QColor(255, 200, 200)); 
        }
        tablaResultados->setItem(i, 2, itemEstado);
    }
}

void EstadisticasWindow::mostrarMejorAlgoritmo(const std::vector<ResultadoAlgoritmo>& resultados)
{
    // Encontrar el algoritmo con mejor (menor) tiempo de espera promedio
    double mejorTiempo = std::numeric_limits<double>::max();
    QString mejorAlgoritmo = "";
    
    for (const auto& resultado : resultados) {
        if (resultado.ejecutado && resultado.tiempoEsperaPromedio < mejorTiempo) {
            mejorTiempo = resultado.tiempoEsperaPromedio;
            mejorAlgoritmo = resultado.nombre;
        }
    }
    
    if (!mejorAlgoritmo.isEmpty()) {
        labelMejorAlgoritmo->setText(QString("🏆 MEJOR ALGORITMO: %1 (Tiempo promedio: %2)")
                                   .arg(mejorAlgoritmo)
                                   .arg(QString::number(mejorTiempo, 'f', 2)));
    } else {
        labelMejorAlgoritmo->setText("No se pudo determinar el mejor algoritmo");
    }
}

void EstadisticasWindow::onAbrirResultadosEstadisticasClicked() {
    // 1) Ruta absoluta a build/
    QString rutaSalida = QDir::current().absoluteFilePath("resultados_estadisticas.txt");

    // 2) Verificar existencia
    if (!QFile::exists(rutaSalida)) {
        QMessageBox::warning(this, "Archivo no encontrado",
                             "No existe 'resultados_estadisticas.txt'. Ejecuta primero Calcular Estadísticas.");
        return;
    }

    // 3) Leer contenido del TXT
    QFile file(rutaSalida);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error al leer archivo",
                             QString("No se pudo abrir '%1' para lectura.").arg(rutaSalida));
        return;
    }
    QTextStream in(&file);
    QString contenido = in.readAll();
    file.close();

    // 4) Mostrar en un diálogo con QTextEdit
    QDialog dialog(this);
    dialog.setWindowTitle("Resultados Estadísticas");
    dialog.resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QTextEdit *textEdit = new QTextEdit(&dialog);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(contenido);
    layout->addWidget(textEdit);

    dialog.exec();
}