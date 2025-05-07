#include "proceso.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

std::vector<Proceso> cargarProcesosDesdeArchivo(const std::string& ruta) {
    std::vector<Proceso> procesos;
    std::ifstream archivo(ruta);
    std::string linea;

    while (std::getline(archivo, linea)) {
        // Ignora líneas vacías o comentarios
        if (linea.empty() || linea[0] == '#') continue;

        std::replace(linea.begin(), linea.end(), ',', ' ');
        std::istringstream ss(linea);
        Proceso p;

        if (ss >> p.pid >> p.burstTime >> p.arrivalTime >> p.priority) {
            procesos.push_back(p);
        } else {
            std::cerr << "Error en línea: " << linea << std::endl;
        }
    }

    return procesos;
}