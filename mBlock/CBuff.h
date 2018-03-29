#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H
#include "Defines.h"
#include <Arduino.h>

template<typename T, bool useTailParam = false>
class CircularBuffer
{
  private:
    int capacity;
    int head;
    int tail;
    T* data;
    
  public:
    CircularBuffer(int capacity, T* data); 
    void push(T data);
    T access(int);
    T pop();
    bool empty();
};

template<typename T, bool useTailParam>
CircularBuffer<T, useTailParam>::CircularBuffer(int capacity, T* data)
{
  this->data = data;
  this->capacity = capacity;
  this->head = 0;
  this->tail = 0;
}

template<typename T, bool useTailParam>
void CircularBuffer<T, useTailParam>::push(T data)
{
  if(useTailParam)
  {
    int next = this->head + 1;
    if(next >= this->capacity)
      next = 0;
    if(next == this->tail)
      return;
  }
  
  *(this->data + this->head) = data; //
  this->head++; // increment head by one
  if(head >= this->capacity)
  {
    this->head = 0; // set it back to 0 if it it has exceded the capacity
  }
}

template<typename T, bool useTailParam>
bool CircularBuffer<T, useTailParam>::empty()
{
  if(!useTailParam)
    return false;
  else
    return (this->tail == this->head);
}

template<typename T, bool useTailParam>
T CircularBuffer<T, useTailParam>::pop()
{
  if(!useTailParam)
    return (T)0;

  if(this->tail == this->head)
    return (T)0;
  
  T ret = *(this->data + this->tail);
  this->tail++;
  if(this->tail >= this->capacity)
  {
    this->tail = 0;
  }

  return ret;
}
      // [-3] [-2]  [-1]  [0]  [1]  [2]  [3]  [4]  [5] 
      //  [5]  [4]   [3]  [2]  [1]  [0]  [H]  [4]  [3] 
template<typename T, bool useTailParam>
T CircularBuffer<T, useTailParam>::access(int index)
{
    {
      int temp = this->head - (index + 1);
      if(temp < 0)
      {
        temp += this->capacity;
      }
      return(*(this->data + temp)); // return memory box accessed by current pointer
    }
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
