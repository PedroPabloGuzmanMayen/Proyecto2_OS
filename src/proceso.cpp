#include "proceso.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

std::vector<Proceso> cargarProcesosDesdeArchivo(const QString &ruta) {
    std::vector<Proceso> procesos;
    QFile archivo(ruta);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo:" << ruta;
        return procesos;
    }

    QTextStream in(&archivo);
    while (!in.atEnd()) {
        QString linea = in.readLine().trimmed();
        if (linea.isEmpty()) continue;
        QStringList partes = linea.split(",");
        if (partes.size() >= 4) {
            Proceso p;
            p.pid = partes[0].trimmed();
            p.burstTime = partes[1].trimmed().toInt();
            p.arrivalTime = partes[2].trimmed().toInt();
            p.priority = partes[3].trimmed().toInt();
            procesos.push_back(p);
        } else {
            qDebug() << "Formato incorrecto en línea:" << linea;
        }
    }
    archivo.close();
    return procesos;
}