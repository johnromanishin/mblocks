#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H
#include "defines.h"
#include <Arduino.h>

template<class T>
class CircularBuffer
{
  private:
    int capacity;
    int head;
    T* data;
    
  public:
    CircularBuffer(int capacity, T* data); 
    void push(T data);
    T access(int);
};

#endif
/**
 * int bufspace[128];
 * CircularBuffer<int> cb(128, &bufspace[0]);
 * cb.push(53);
 * cb.push(43);
 * cb.access(0);  <--- returns 43
 * cb.access(1);  <--- returns 53
 */
/**
 * int bufspace[128];
 * CircularBuffer<int> cb(128, &bufspace[0]);
 * cb.push(53);
 * cb.push(43);
 * cb.access(0);  <--- returns 43
 * cb.access(1);  <--- returns 53
 */
