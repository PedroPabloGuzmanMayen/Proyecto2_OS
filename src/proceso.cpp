#include "proceso.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <unordered_set>
#include <QMessageBox>

std::vector<Proceso> cargarProcesosDesdeArchivo(const QString &ruta) {
    std::vector<Proceso> procesos;
    std::unordered_set<QString> seenPIDs;
    QStringList errores;  // <-- acumulador de mensajes de error

    QFile archivo(ruta);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", QString("No se pudo abrir el archivo:\n%1").arg(ruta));
        return procesos;
    }

    QTextStream in(&archivo);
    int lineaNum = 0;
    while (!in.atEnd()) {
        QString linea = in.readLine().trimmed();
        lineaNum++;
        if (linea.isEmpty()) continue;

        QStringList partes = linea.split(",");
        if (partes.size() < 4) {
            errores.append(QString("Formato incorrecto en línea %1 : \"%2\"")
                          .arg(lineaNum).arg(linea));
            continue;
        }

        QString pidStr    = partes[0].trimmed();
        bool    okBurst   = false;
        bool    okArrival = false;
        bool    okPrio    = false;
        int     burstVal   = partes[1].trimmed().toInt(&okBurst);
        int     arrivalVal = partes[2].trimmed().toInt(&okArrival);
        int     prioVal    = partes[3].trimmed().toInt(&okPrio);

        if (pidStr.isEmpty()) {
            errores.append(QString("PID vacío en línea %1 : \"%2\"").arg(lineaNum).arg(linea));
            continue;
        }
        if (!okBurst || burstVal <= 0) {
            errores.append(QString("BurstTime inválido en línea %1 : \"%2\"")
                          .arg(lineaNum).arg(partes[1].trimmed()));
            continue;
        }
        if (!okArrival || arrivalVal < 0) {
            errores.append(QString("ArrivalTime inválido en línea %1 : \"%2\"")
                          .arg(lineaNum).arg(partes[2].trimmed()));
            continue;
        }
        if (!okPrio || prioVal < 0) {
            errores.append(QString("Priority inválido en línea %1 : \"%2\"")
                          .arg(lineaNum).arg(partes[3].trimmed()));
            continue;
        }
        if (seenPIDs.find(pidStr) != seenPIDs.end()) {
            errores.append(QString("Proceso duplicado detectado: PID \"%1\" en línea %2")
                          .arg(pidStr).arg(lineaNum));
            continue;
        }

        Proceso p;
        p.pid            = pidStr;
        p.burstTime      = burstVal;
        p.arrivalTime    = arrivalVal;
        p.priority       = prioVal;
        p.startTime      = -1;
        p.completionTime = -1;
        p.waitingTime    = 0;
        p.turnaroundTime = 0;

        procesos.push_back(p);
        seenPIDs.insert(pidStr);
    }

    archivo.close();

    if (!errores.isEmpty()) {
        QMessageBox::warning(nullptr, "Errores en procesos.txt", errores.join("\n"));
        return {};
    }

    return procesos;
}