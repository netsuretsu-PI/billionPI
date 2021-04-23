#include "wrappedVector.hpp"

template<class T>
ull WrappedVector<T>::push_back(const T& v){
    vec.push_back(v);
    this->_size++;
    return vec.size();
}

template<class T>
ull WrappedVector<T>::pop_back(){
    vec.pop_back();
    this->_size--;
    return vec.size();
}

template<class T>
T& WrappedVector<T>::operator[](ull idx){
    return vec[idx];
}

template<class T>
void WrappedVector<T>::resize(ull size){
    vec.resize(size);
}
