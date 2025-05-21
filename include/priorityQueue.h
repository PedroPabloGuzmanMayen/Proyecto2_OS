#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <iostream>
#include <vector>
#include <string>
#include "proceso.h"

using namespace std;
class priorityQueue {
    public:
        void insertNewValue(); // Método para insertar un nuevo valor
        void push(); //Eliminar el elemento con mayor prioridad
        void deleteValue(); // Eliminar un nodo el queue
        void goUp(); // Si un nuevo elemento insertado  viola el constraint del heap 

    private: 
        void getParent(int index); //Obtener el nodo padre de un nodo
        void getSon(int position, int index); //Obtener el hijo izquierdo o derecho de un nodo
        vector<Proceso> heap;  //Vector con los procesos que vamos a utilizar

};

#endif