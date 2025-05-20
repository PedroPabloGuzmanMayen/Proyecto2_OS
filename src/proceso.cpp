#include "proceso.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

std::vector<Proceso> cargarProcesosDesdeArchivo(const QString &ruta) {
    std::vector<Proceso> procesos;
    
    // Abrir el archivo
    QFile archivo(ruta);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo:" << ruta;
        return procesos;
    }
    
    QTextStream in(&archivo);
    
    // Leer el archivo línea por línea
    while (!in.atEnd()) {
        QString linea = in.readLine().trimmed();
        
        // Ignorar líneas vacías
        if (linea.isEmpty()) {
            continue;
        }
        
        // Dividir la línea por comas
        QStringList partes = linea.split(",");
        
        // Verificar que tenemos los 4 elementos necesarios
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