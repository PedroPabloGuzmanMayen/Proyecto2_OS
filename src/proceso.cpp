#include "proceso.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

std::vector<Proceso> cargarProcesosDesdeArchivo(const QString &ruta) {
    std::vector<Proceso> procesos;
    QFile file(ruta);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo:" << ruta;
        return procesos;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) continue;

        QStringList tokens = line.split(",");
        if (tokens.size() == 4) {
            Proceso p;
            p.pid = tokens[0].trimmed();
            p.burstTime = tokens[1].trimmed().toInt();
            p.arrivalTime = tokens[2].trimmed().toInt();
            p.priority = tokens[3].trimmed().toInt();
            procesos.push_back(p);
        }
    }

    return procesos;
}