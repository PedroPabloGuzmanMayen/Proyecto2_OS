#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <vector>
#include <QString>
#include "proceso.h"

// Cola de prioridad mínima por burstTime
class priorityQueue {
public:
    priorityQueue();
    ~priorityQueue();

    void insertNewValue(const Proceso& proceso);
    Proceso top();
    Proceso pop();
    bool deleteValue(const QString& pid);
    bool isEmpty();
    int size();
    void printQueue();
    void clear();

private:
    std::vector<Proceso> heap;

    int getParent(int index);
    int getLeftChild(int index);
    int getRightChild(int index);
    bool hasLeftChild(int index);
    bool hasRightChild(int index);
    bool hasParent(int index);
    void swap(int index1, int index2);
    void goUp();
    void goDown();
    void goUpFrom(int index);
    void goDownFrom(int index);
};

#endif // PRIORITYQUEUE_H