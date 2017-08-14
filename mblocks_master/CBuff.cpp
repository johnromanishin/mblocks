#include "CBuff.h"

template<typename T>
CircularBuffer<T>::CircularBuffer(int capacity, T* data)
{
  this->data = data;
  this->capacity = capacity;
  this->head = 0;
}

template<typename T>
void CircularBuffer<T>::push(T data)
{
  *(this->data + this->head) = data; //
  this->head++; // increment head by one
      if(head >= this->capacity)
      {
        this->head = 0; // set it back to 0 if it it has exceded the capacity
      }
}
      // [-3] [-2]  [-1]  [0]  [1]  [2]  [3]  [4]  [5] 
      //  [5]  [4]   [3]  [2]  [1]  [0]  [H]  [4]  [3] 
template<typename T>
T CircularBuffer<T>::access(int index)
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
/**
 * int bufspace[128];
 * CircularBuffer<int> cb(128, &bufspace[0]);
 * cb.push(53);
 * cb.push(43);
 * cb.access(0);  <--- returns 43
 * cb.access(1);  <--- returns 53
 */
