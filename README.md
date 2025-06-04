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

Si su distribución usa otro gestor de paquetes, asegúrense de instalar `qtbase5-dev` (o equivalente) para que esté disponible `Qt5Widgets`.

---

## ⚙️ Compilación (Opción 1)

1. Clone o descargue este repositorio:

   ```bash
   git clone https://github.com/PedroPabloGuzmanMayen/Proyecto2_OS.git
   cd Proyecto2_OS
   ```
2. Cree y entre a la carpeta de compilación:

   ```bash
   mkdir build
   cd build
   ```
3. Configure el proyecto con CMake y compílalo:

   ```bash
   cmake ..
   make
   ```

   Esto generará los ejecutables `simulador` y `tests` dentro de `build/`.

### 🔁 Opción 2: **Recrear la carpeta `build/` desde cero**

(Si se cambian rutas, archivos `.h` con `Q_OBJECT`, o se tienen errores raros), haga lo siguiente:

```bash
cd Proyecto2_OS
rm -rf build
mkdir build
cd build
cmake ..
make
```

---

## ▶️ Ejecución del simulador

Desde la carpeta `build/`, ejecute:

```bash
./simulador
```

Se abrirá una ventana con un menú principal con dos opciones:

* **Opción 1 del menú principal - Simulador visual**

Se abrirá una ventana con:

* **Simulación A (Calendarización de procesos):**

  * Selector de algoritmo (FIFO, SJF, SRT, Round Robin, Priority).
  * Campo “Quantum” (visible solo si elige Round Robin).
  * Botones para “Simulación A” y “Abrir resultados Sim A”.
    
* **Simulación B (Mutex / Semáforos):**

  * Rutas para tres archivos de entrada:

    1. `procesos.txt` (lista de procesos con PID, duración, llegada, prioridad)
    2. `recursos.txt` (lista de recursos con nombre y contador)
    3. `acciones.txt` (acciones de cada proceso sobre los recursos)
  * Opción de “Mutex Locks” o “Semáforo”.
  * Botones “Ver Procesos”, “Ver Recursos”, “Ver Acciones”, “Limpiar Sim B” y “Simulación B”.

* **Opción 2 del menú principal - Calcular estadísticas de múltiples algoritmos**

  * Selector múltiple de algoritmos (FIFO, SJF, SRT, Round Robin, Priority).
  * Botón calcular estadísticas (muestra las estadísticas de cada uno de los algortimos seleccionados previamente y muestra el mejor algoritmo con el mejor tiempo promedio).

---

## ▶️ Ejecución de Tests

Desde la carpeta `build/`, ejecute:

```bash
./tests
```

**Descripción de `tests.cpp`:**
El archivo `tests.cpp` verifica el correcto funcionamiento de todos los algoritmos y mecanismos implementados en el simulador. Cada test hace lo siguiente:

1. **FIFO simple:**

    * Crea tres procesos con diferentes tiempos de llegada y ráfaga.
    * Invoca `fifo(procesos, nullptr)` y comprueba que el orden de salida sea `P1, P2, P3` (primer llegado, primer servido).
      
 2. **SJF simple:**

    * Crea tres procesos cuyos tiempos de ráfaga son 5, 3 y 4.
    * Invoca `shortestJobFirst(procesos, nullptr)` y comprueba que la secuencia de PIDs siga el orden de ráfaga ascendente: `P2 (3)`, `P3 (4)`, `P1 (5)`.
      
 3. **Priority simple:**

    * Crea tres procesos con prioridades 2, 1 y 3.
    * Invoca `priorityScheduling(procesos, nullptr)` y comprueba que la secuencia de PIDs siga el orden de prioridad ascendente: `P2 (1)`, `P1 (2)`, `P3 (3)`.
      
 4. **Round Robin simple:**

    * Crea tres procesos con ráfagas 5, 3 y 4 y asigna un quantum de 2.
    * Invoca `roundRobin(procesos, quantum, dummy, nullptr)`.
    * Filtra la primera aparición de cada PID en la lista completa de ejecuciones y comprueba que el orden de inicio sea `P1, P2, P3`.
      
 5. **SRT simple:**

    * Crea tres procesos con ráfagas 5, 3 y 4.
    * Invoca `shortestRemainingTime(procesos, bloques, nullptr)` y comprueba que la secuencia final de procesos sea `P2, P1, P3` (el proceso de ráfaga más corta se ejecuta primero, con preempción).
      
 6. **Mutex simple:**

    * Crea dos procesos que compiten por un recurso con capacidad 1.
    * Define dos acciones de tipo READ en el mismo ciclo para `P1` y `P2`.
    * Invoca `simulateMutex(acciones, recursos, procesos, nullptr, nullptr)` y comprueba que la línea de tiempo resulte en tres bloques:

      1. `P1 ACCESS`
      2. `P2 WAIT`
      3. `P2 ACCESS`
         
 7. **Semáforo simple:**

    * Crea dos procesos que compiten por un recurso con capacidad 2.
    * Define dos acciones de tipo READ en el mismo ciclo para `P1` y `P2`.
    * Invoca `simulateSyncSemaforo(acciones, recursos, procesos, nullptr, nullptr)` y comprueba que ambos accedan sin esperas, resultando en dos bloques `ACCESS`.

 Al finalizar, muestra un resumen con la cantidad de tests pasados y fallidos.

---

## 📂 Archivos de entrada

### Simulación A: `data/procesos.txt`

Ubicado en `data/`, este archivo contiene la lista de procesos con el formato CSV:

```
<PID>,<DURACIÓN>,<LLEGADA>,<PRIORIDAD>
```

**Ejemplo:**

```txt
P1,5,0,2
P2,3,1,1
P3,8,2,3
P4,6,3,2
```

* **PID**: identificador único (no puede repetirse ni estar vacío).
* **DURACIÓN**: burstTime > 0 (entero).
* **LLEGADA**: arrivalTime ≥ 0 (entero).
* **PRIORIDAD**: ≥ 0 (entero).

Si el archivo contiene líneas mal formateadas, valores no numéricos o valores fuera de rango (burstTime ≤ 0, arrivalTime < 0, prioridad < 0) o PIDs duplicados, al hacer clic en **Simulación A** se mostrará un único cuadro emergente (`QMessageBox::warning`) con todos los errores detectados (por ejemplo:

```
BurstTime inválido en línea 2 : "-3"
ArrivalTime inválido en línea 3 : "mal"
Formato incorrecto en línea 4 : "P4,6,3"
Proceso duplicado detectado: PID "P1" en línea 5
Priority inválido en línea 6 : "–1"
```

) y la simulación no avanzará hasta que el archivo sea corregido.

---

### Simulación B:

Hay tres archivos de entrada, todos ubicados en `data/` por defecto:

1. **`procesos.txt`**
   Mismo formato que en la Simulación A:

   ```txt
   <PID>,<DURACIÓN>,<LLEGADA>,<PRIORIDAD>
   ```

   Validaciones:

   * PID no vacío ni repetido.
   * burstTime > 0, arrivalTime ≥ 0, prioridad ≥ 0.
   * Si hay errores, al hacer “Simulación B” se mostrará un único `QMessageBox::warning` con todos los mensajes y no avanzará.

2. **`recursos.txt`**
   Cada línea:

   ```txt
   <NOMBRE>,<COUNT>
   ```

   * **NOMBRE**: cadena no vacía, no repetida.
   * **COUNT**: entero válido ≥ 0 (si < 0 o no numérico, produce error).
     Ejemplo:

   ```txt
   R1,1
   R2,2
   R3,1
   ```

   * Si hay errores (formato incorrecto, nombre vacío, cantidad negativa, duplicados), aparece un `QMessageBox::warning` con todos los errores y la simulación se detiene.

3. **`acciones.txt`**
   Cada línea:

   ```txt
   <PID>,<ACTION>,<RECURSO>,<CICLO>
   ```

   * **PID**: debe existir en el `procesos.txt` cargado.
   * **ACTION**: “READ” o “WRITE” (coma sensible a mayúsculas).
   * **RECURSO**: nombre de recurso no vacío (debe existir en `recursos.txt`).
   * **CICLO**: entero ≥ 0.
   * No pueden repetirse tres acciones con el mismo pid, action, recurso y ciclo (clave única).
   * Si hay errores de formato o valores inválidos o duplicados, se muestra un `QMessageBox::warning` con todos los errores y se detiene la simulación.

Además, tras cargar correctamente estos tres archivos, la función interna `validateAndSortActions` verifica que para cada acción `cycle ≥ arrivalTime(proceso)`. Si alguna acción viola esto o referencia un PID    que no existe, se lanza un `QMessageBox::critical` con la lista de errores de validación y no avanza.

---

## 📈 Visualización y flujo

### 1. Simulación A (algoritmos de planificación)

1. Seleccioné o ingresé la ruta de tu `procesos.txt`.
2. Elija un algoritmo (FIFO, SJF, SRT, Round Robin, Priority).

   * Si se elije **Round Robin**, aparecerá automáticamente el campo **Quantum**. Debe ser entero > 0.
   * Si `quantum ≤ 0`, aparece un `QMessageBox::warning` (“Quantum inválido…”) y la simulación no arranca.
     
3. Haga clic en **Simulación A**.

   * Si el archivo tiene errores, verás un `QMessageBox` con la lista y no se dibuja nada.
   * Si todo es válido, se abrirá un panel **GanttWindow** donde, paso a paso (con delay de \~300 ms por ciclo), se irá dibujando cada bloque de CPU para cada proceso.
   * Al finalizar, se genera `build/resultados_simA.txt` con formato CSV:

     ```
     PID,StartTime,CompletionTime,WaitingTime,TurnaroundTime
     P1,0,5,0,5
     P2,5,8,4,7
     ...
     ```
   * Además, aparece un cuadro emergente (`QMessageBox`) que muestra el orden de ejecución y el tiempo de espera promedio.
   * Puede pulsar **“Abrir resultados Simulación A”** para ver ese archivo en un diálogo de texto.

### 2. Simulación B (mutex / semáforos)

1. Selecciona o usa por defecto los tres archivos (`procesos.txt`, `recursos.txt`, `acciones.txt`).
2. Elige **Mutex Locks** o **Semáforo**.
3. Haga clic en **Simulación B**.

   * Si alguno de los tres archivos tiene errores (mal formato, valores inválidos, duplicados, acción con ciclo < arrivalTime, recurso desconocido, PID no encontrado), verás un **único** `QMessageBox::warning` (o `QMessageBox::critical` si la violación ocurre en validación de acciones).
   * La simulación se detiene y no se dibuja nada.
     
4. Si todo es válido, aparecerá un `GanttWindow` y, para cada acción, se ejecutará:

   1. Se libera cualquier acceso terminado antes del ciclo actual.
   2. Si el recurso está ocupado (used ≥ capacity), se genera un bloque de **WAIT** (duración = nextFree – requestedCycle), que se dibuja paso a paso en rojo.
   3. Luego se genera un bloque de **ACCESS** (duración = 1) en verde, que también se dibuja.
   4. Se registra en `endTimes` cuándo se libera (start + 1).
      
5. Al terminar, se muestra un cuadro con formato HTML que lista cada bloque en orden:

   ```
   P1 | R1 | Ciclo: 0 | <span style='color:green; font-weight:bold;'>ACCESS</span><br>
   P2 | R2 | Ciclo: 1 | <span style='color:red; font-weight:bold;'>WAIT</span><br>
   ...
   ```

---

## ✍️ Añadidos de “Programación Defensiva”

1. **Detección de errores en archivos** (`.txt` de procesos, recursos, acciones):

   * Se valida formato de cada línea, conversiones numéricas, valores fuera de rango, duplicados.
   * Si se detecta **al menos un** error, se construye un `QStringList` con mensajes detallados (por línea):

     * Ejemplos en `proceso.cpp`:

       ```
       BurstTime inválido en línea 2 : "-3"
       ArrivalTime inválido en línea 3 : "mal"
       Proceso duplicado detectado: PID "P1" en línea 5
       ```
     * Ejemplos en `synchronizer.cpp` (recursos.txt):

       ```
       Cantidad negativa para recurso "R2" en línea 2
       Recurso duplicado detectado: "R2" en línea 3
       ```
     * Ejemplos en `synchronizer.cpp` (acciones.txt):

       ```
       Tipo de acción no reconocido en línea 4 : "EDIT"
       Acción duplicada detectada en línea 5 : PID="P1" ACTION="READ" RECURSO="R1" CICLO=0
       Proceso P2 no encontrado para acción en ciclo 1
       ```
   * Al final de la carga de cada archivo, si hay errores, se muestra un **solo** `QMessageBox::warning` (o `critical` en validación de acciones) con la lista completa. La simulación **no continúa** hasta corregirlos.

2. **Validación de `cycle ≥ arrivalTime` en Sim B**:

   * Si una acción se programa antes de que el proceso exista, `validateAndSortActions(...)` agrega un mensaje al acumulador y, al detectar cualquiera, muestra un `QMessageBox::critical` con todos los errores y retorna `false`, abortando la simulación.

3. **Chequeos en Sim A**:

   * Se verifica que el vector de procesos no esté vacío y que cada campo (burstTime, arrivalTime, priority) sea válido.
   * Si elige Round Robin, se comprueba `quantum > 0`. En caso contrario, se muestra un `QMessageBox::warning` y se detiene.

---

## 📁 Estructura del proyecto

```
Proyecto2_OS/
├── gui/                  # Códigos de la interfaz gráfica con Qt
│   ├── gui.cpp           # Implementación de la ventana principal y slots
│   ├── gui.h             # Declaración de SimuladorGUI
│   ├── mainmenu.cpp      # Subventana de menú inicial (si aplica)
│   ├── mainmenu.h        # Ventana del menú principal del programa
│   ├── estadisticas.cpp  # Ventana de estadísticas
│   └── estadisticas.h    # Headers de estadísticas
├── include/              # Headers públicos
│   ├── algoritmo.h       # Declaración de algoritmos de planificación
│   ├── proceso.h         # Estructura Proceso
│   ├── ganttwindow.h     # Declaración de GanttWindow (Qt Widget)
│   ├── priorityQueue.h   # Estructura de cola mínima usada en SRT
│   ├── sincronizer.h     # Declaración de funciones de Simulación B
│   ├── tipOS.h           # Tipos compartidos (BloqueGantt, BloqueSync, etc.)
│   └── tipos.h           # (Alias de BloqueSync, si aplica)
├── src/                  # Lógica de simulación y ejecución
│   ├── main.cpp          # `main()` que muestra SimuladorGUI
│   ├── algoritmo.cpp     # Implementación de algoritmos (FIFO, SJF, RR, SRT, Priority) con defensivas
│   ├── proceso.cpp       # Función cargarProcesosDesdeArchivo(...) con validaciones y pop-ups
│   ├── ganttwindow.cpp   # Implementación de la ventana de Gantt (dibujar bloques)
│   ├── synchronizer.cpp  # Implementación de loadRecursos, loadAcciones y simulateSync con defensivas
│   ├── priorityQueue.cpp # Implementación de cola mínima usada en SRT
│   └── proceso.cpp       # (Repetido) Manejador de lectura de procesos
├── test/                 # Pruebas unitarias de los algoritmos de planificación
│   └── tests.cpp         # Casos de prueba para FIFO, SJF, SRT, RR, Priority (Simulación A) y casos de prueba para Mutex Lock y semáforos (Simulación B)
├── data/                 # Archivos de entrada por defecto
│   ├── procesos.txt      # Lista de procesos (sim A y B)
│   ├── recursos.txt      # Lista de recursos (sim B)
│   └── acciones.txt      # Acciones sobre recursos (sim B)
├── build/                # Carpeta generada por CMake (ignorado en Git)
├── CMakeLists.txt        # Archivo principal de CMake
└── README.md             # Este archivo
```

---

## 📚 Autores

* **Pablo Daniel Barillas**
  
* **Pedro Pablo Guzmán**
  
* **Esteban Zambrano**

Proyecto universitario – Universidad del Valle de Guatemala

Curso: **Sistemas Operativos**, sección: **40**

Docente: **Sebastián Galindo**

Año: **2025**
