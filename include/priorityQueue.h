#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "proceso.h"

using namespace std;

class priorityQueue {
public:
    // Constructor y destructor
    priorityQueue();
    ~priorityQueue();
    // Métodos principales
    void insertNewValue(const Proceso& proceso); //Ayuda a insertar un nuevo proceso
    Proceso pop(); //Elimina el proceso con mayor prioridad
    Proceso top(); //Ayuda a encontrar el proceso con más prioridad
    bool deleteValue(const QString& pid); //Nos ayuda a eliminar un proceso en base a su ID
    bool isEmpty(); //Verifica si la queue está vacía
    int size(); //Obtienen el tamañao
    void printQueue();//Será de ayuda para debuguear el proceso
    void clear();//Limpia la queue

private:
    // Métodos para manejo del heap
    int getParent(int index); // Obtener índice del padre
    int getLeftChild(int index); // Obtener índice del hijo izquierdo
    int getRightChild(int index); // Obtener índice del hijo derecho
    bool hasParent(int index);// Verificar si un nodo tiene padre
    bool hasLeftChild(int index);// Verificar si tiene hijo izquierdo
    bool hasRightChild(int index);  // Verificar si tiene hijo derecho
    void swap(int index1, int index2); // Intercambiar dos elementos
    void goUp();// Reorganizar hacia arriba (último elemento)
    void goDown();// Reorganizar hacia abajo (desde raíz)
    void goUpFrom(int index);// Reorganizar hacia arriba desde índice específico
    void goDownFrom(int index);// Reorganizar hacia abajo desde índice específico
    
    vector<Proceso> heap;// Vector que representa el heap
};

#endif