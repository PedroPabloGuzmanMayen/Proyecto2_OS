#include "priorityQueue.h"
#include <stdexcept>
#include <iostream>

priorityQueue::priorityQueue() {}
priorityQueue::~priorityQueue() {}

// Índice del padre
int priorityQueue::getParent(int index) {
    if (index == 0) return -1;
    return (index - 1) / 2;
}

// Índice hijo izquierdo
int priorityQueue::getLeftChild(int index) {
    return 2 * index + 1;
}

// Índice hijo derecho
int priorityQueue::getRightChild(int index) {
    return 2 * index + 2;
}

bool priorityQueue::hasLeftChild(int index) {
    return getLeftChild(index) < heap.size();
}
bool priorityQueue::hasRightChild(int index) {
    return getRightChild(index) < heap.size();
}
bool priorityQueue::hasParent(int index) {
    return getParent(index) >= 0;
}

void priorityQueue::swap(int index1, int index2) {
    Proceso temp = heap[index1];
    heap[index1] = heap[index2];
    heap[index2] = temp;
}

void priorityQueue::goUp() {
    int index = heap.size() - 1;
    while (hasParent(index) && heap[getParent(index)].burstTime > heap[index].burstTime) {
        swap(getParent(index), index);
        index = getParent(index);
    }
}

void priorityQueue::goDown() {
    int index = 0;
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

void priorityQueue::insertNewValue(const Proceso& proceso) {
    heap.push_back(proceso);
    goUp();
}

Proceso priorityQueue::top() {
    if (heap.empty()) {
        throw std::runtime_error("Priority queue está vacía");
    }
    return heap[0];
}

Proceso priorityQueue::pop() {
    if (heap.empty()) {
        throw std::runtime_error("Priority queue está vacía");
    }
    Proceso item = heap[0];
    heap[0] = heap[heap.size() - 1];
    heap.pop_back();
    goDown();
    return item;
}

bool priorityQueue::deleteValue(const QString& pid) {
    for (int i = 0; i < heap.size(); i++) {
        if (heap[i].pid == pid) {
            heap[i] = heap[heap.size() - 1];
            heap.pop_back();
            if (i < heap.size()) {
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
    return false;
}

void priorityQueue::goUpFrom(int index) {
    while (hasParent(index) && heap[getParent(index)].burstTime > heap[index].burstTime) {
        swap(getParent(index), index);
        index = getParent(index);
    }
}

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

bool priorityQueue::isEmpty() {
    return heap.empty();
}

int priorityQueue::size() {
    return heap.size();
}

void priorityQueue::printQueue() {
    std::cout << "Priority Queue (heap): ";
    for (int i = 0; i < heap.size(); i++) {
        std::cout << "[" << heap[i].pid.toStdString()
                  << ",P:" << heap[i].burstTime << "] ";
    }
    std::cout << std::endl;
}

void priorityQueue::clear() {
    heap.clear();
}