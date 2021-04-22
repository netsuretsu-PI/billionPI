#include "longVector.hpp"

template <class T>
ull LongVector<T>::size() {
    return _size;
}

template <class T>
LongVector<T>::LongVector(ull _size){
    this->_size = _size;
}

template <class T>
LongVector<T>::~LongVector(){
    
}