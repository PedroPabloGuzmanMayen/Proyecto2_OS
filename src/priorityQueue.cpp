#include "priorityQueue.h"

// Constructor
priorityQueue::priorityQueue() {}

// Destructor
priorityQueue::~priorityQueue() {}

// Obtener el índice del padre
int priorityQueue::getParent(int index) {
    if (index == 0) return -1; // La raíz no tiene padre
    return (index - 1) / 2;
}

// Obtener el índice del hijo izquierdo
int priorityQueue::getLeftChild(int index) {
    return 2 * index + 1;
}

// Obtener el índice del hijo derecho  
int priorityQueue::getRightChild(int index) {
    return 2 * index + 2;
}

// Verificar si tiene hijo izquierdo
bool priorityQueue::hasLeftChild(int index) {
    return getLeftChild(index) < heap.size();
}

// Verificar si tiene hijo derecho
bool priorityQueue::hasRightChild(int index) {
    return getRightChild(index) < heap.size();
}

// Verificar si tiene padre
bool priorityQueue::hasParent(int index) {
    return getParent(index) >= 0;
}

// Intercambiar dos elementos en el heap
void priorityQueue::swap(int index1, int index2) {
    Proceso temp = heap[index1];
    heap[index1] = heap[index2];
    heap[index2] = temp;
}

// Reorganizar hacia arriba al hacer una inserción
void priorityQueue::goUp() {
    int index = heap.size() - 1;
    
    // Mientras tenga padre y la prioridad sea mayor (número menor = mayor prioridad)
    while (hasParent(index) && heap[getParent(index)].burstTime > heap[index].burstTime) {
        swap(getParent(index), index);
        index = getParent(index);
    }
}

// Reorganizar hacia abajo cuando eliminamos
void priorityQueue::goDown() {
    int index = 0;
    
    // Mientras tenga al menos un hijo
    while (hasLeftChild(index)) {
        int smallerChildIndex = getLeftChild(index);
        
        // Si tiene hijo derecho y es de mayor prioridad que el izquierdo
        if (hasRightChild(index) && 
            heap[getRightChild(index)].burstTime < heap[getLeftChild(index)].burstTime) {
            smallerChildIndex = getRightChild(index);
        }
        
        // Si el elemento actual ya tiene mayor prioridad que sus hijos, está en su lugar
        if (heap[index].burstTime < heap[smallerChildIndex].burstTime) {
            break;
        } else {
            swap(index, smallerChildIndex);
        }
        
        index = smallerChildIndex;
    }
}

// Insertar un nuevo proceso
void priorityQueue::insertNewValue(const Proceso& proceso) {
    heap.push_back(proceso);
    goUp();
}

// Obtener el proceso con mayor prioridad (sin eliminarlo)
Proceso priorityQueue::top() {
    if (heap.empty()) {
        throw runtime_error("Priority queue está vacía");
    }
    return heap[0];
}

// Eliminar y retornar el proceso con mayor prioridad
Proceso priorityQueue::pop() {
    if (heap.empty()) {
        throw runtime_error("Priority queue está vacía");
    }
    
    Proceso item = heap[0];
    heap[0] = heap[heap.size() - 1];
    heap.pop_back();
    goDown();
    return item;
}

// Eliminar un proceso específico por PID
bool priorityQueue::deleteValue(const QString& pid) {
    for (int i = 0; i < heap.size(); i++) {
        if (heap[i].pid == pid) {
            // Reemplazar con el último elemento
            heap[i] = heap[heap.size() - 1];
            heap.pop_back();
            
            // Reorganizar el heap
            if (i < heap.size()) {
                // Intentar subir primero
                int parent = getParent(i);
                if (parent >= 0 && heap[parent].burstTime > heap[i].burstTime) {
                    goUpFrom(i);
                } else {
                    goDownFrom(i);
                }
            }
            return true;
        }
    }
    return false; // No se encontró el proceso
}

// Reorganizar hacia arriba desde un índice específico
void priorityQueue::goUpFrom(int index) {
    while (hasParent(index) && heap[getParent(index)].burstTime > heap[index].burstTime) {
        swap(getParent(index), index);
        index = getParent(index);
    }
}

// Reorganizar hacia abajo desde un índice específico
void priorityQueue::goDownFrom(int index) {
    while (hasLeftChild(index)) {
        int smallerChildIndex = getLeftChild(index);
        
        if (hasRightChild(index) && 
            heap[getRightChild(index)].burstTime < heap[getLeftChild(index)].burstTime) {
            smallerChildIndex = getRightChild(index);
        }
        
        if (heap[index].burstTime < heap[smallerChildIndex].burstTime) {
            break;
        } else {
            swap(index, smallerChildIndex);
        }
        
        index = smallerChildIndex;
    }
}

// Verificar si la queue está vacía
bool priorityQueue::isEmpty() {
    return heap.empty();
}

// Obtener el tamaño de la queue
int priorityQueue::size() {
    return heap.size();
}

// Imprimir la queue (para debugging)
void priorityQueue::printQueue() {
    cout << "Priority Queue (heap): ";
    for (int i = 0; i < heap.size(); i++) {
        cout << "[" << heap[i].pid.toStdString() 
             << ",P:" << heap[i].burstTime << "] ";
    }
    cout << endl;
}

// Limpiar la queue
void priorityQueue::clear() {
    heap.clear();
}