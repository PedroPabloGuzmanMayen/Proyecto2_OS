# 🖥️ Simulador de Sistemas Operativos

Simulador visual desarrollado en C++ con FLTK para representar algoritmos de planificación y mecanismos de sincronización, como parte del curso de **Sistemas Operativos** en la **Universidad del Valle de Guatemala**.

---

## 📦 Requisitos

- CMake >= 3.10
- g++ con soporte para C++17
- FLTK (Fast Light Toolkit)

### ✅ Instalar FLTK (en Ubuntu):

```bash
sudo apt update
sudo apt install libfltk1.3-dev
````

---

## ⚙️ Compilación

1. Clona o descarga este repositorio.
2. Abre terminal en la raíz del proyecto.
3. Ejecuta:

```bash
mkdir build
cd build
cmake ..
make
```

Esto generará el ejecutable `simulador` dentro de `build/`.

---

## ▶️ Ejecución

Desde la carpeta `build/`, ejecuta:

```bash
./simulador
```

Se abrirá una ventana con dos botones:

* **Simulación A:** Carga procesos desde un archivo.
* **Simulación B:** (pendiente de implementación)

---

## 📁 Estructura del proyecto

```
Proyecto2_OS/
├── gui/              # Lógica de interfaz con FLTK
│   ├── gui.cpp
│   └── gui.h
├── src/              # Lógica de simulación y ejecución
│   ├── main.cpp
│   ├── proceso.cpp
├── include/          # Headers públicos
│   └── proceso.h
├── data/             # (Puedes colocar aquí procesos.txt)
├── build/            # Archivos generados por CMake (ignorar en Git)
├── CMakeLists.txt
└── README.md
```

## 📚 Autores
Nombres de los autores: Pablo Daniel Barillas y Pedro Pablo Gúzman
Proyecto universitario - Universidad del Valle de Guatemala
Curso: Sistemas Operativos
Docente: Sebastián Galindo
Año: 2025
