# 🖥️ Simulador de Sistemas Operativos

Simulador visual desarrollado en **C++ con Qt 5** para representar algoritmos de planificación de procesos mediante una interfaz gráfica interactiva con animaciones y diagrama de Gantt. Proyecto realizado como parte del curso de **Sistemas Operativos** en la **Universidad del Valle de Guatemala**.

---

## 📦 Requisitos

- CMake >= 3.10  
- g++ con soporte para C++17  
- Qt5 (Qt5Widgets)

### ✅ Instalar Qt5 (en Ubuntu / WSL):

```bash
sudo apt update
sudo apt install qtbase5-dev
````

---

## ⚙️ Compilación (Opción 1)

1. Clona o descarga este repositorio.
2. Abre una terminal en la raíz del proyecto.
3. Ejecuta:

```bash
mkdir build
cd build
cmake ..
make
```

Esto generará el ejecutable `simulador` dentro de `build/`.

### 🔁 Opción 2: **Recrear la carpeta `build/` desde cero**

(Si se cambian rutas, archivos `.h` con `Q_OBJECT`, o se tienen errores raros), haga lo siguiente:

```bash
cd ..
rm -rf build
mkdir build
cd build
cmake ..
make
```

---

## ▶️ Ejecución

Desde la carpeta `build/`, ejecute:

```bash
./simulador
```

Se abrirá una ventana con:

* Un selector de algoritmo (FIFO, SJF, Round Robin, etc.)
* Campo de Quantum (visible si se elige Round Robin)
* Botones para ejecutar Simulación A o B

---

## 📂 Archivo de entrada: `procesos.txt`

Ubicado en la carpeta `data/`, este archivo contiene la lista de procesos con el siguiente formato:

```txt
PID  DURACIÓN  LLEGADA  PRIORIDAD
P1   5         0        2
P2   3         1        1
P3   8         2        3
P4   6         3        2
```

---

## 📈 Visualización

Al ejecutar una simulación, se mostrará:

* Orden de ejecución del algoritmo seleccionado
* Tiempo de espera promedio
* Animación del **diagrama de Gantt**, que se construye paso a paso con colores por proceso

---

## 📁 Estructura del proyecto

```
Proyecto2_OS/
├── gui/              # Interfaz gráfica con Qt
│   ├── gui.cpp
│   └── gui.h
├── src/              # Lógica de simulación y ejecución
│   ├── main.cpp
│   ├── proceso.cpp
│   ├── algoritmo.cpp
│   └── ganttwindow.cpp
├── include/          # Headers públicos
│   ├── proceso.h
│   ├── algoritmo.h
│   └── ganttwindow.h
├── data/             # Archivo de procesos de entrada
│   └── procesos.txt
├── build/            # Archivos generados por CMake (ignorar en Git)
├── CMakeLists.txt
└── README.md
```

---

## 📚 Autores

**Pablo Daniel Barillas**, **Pedro Pablo Guzmán**, **Esteban Zambrano**
Proyecto universitario – Universidad del Valle de Guatemala
Curso: **Sistemas Operativos**
Docente: **Sebastián Galindo**
Año: **2025**
