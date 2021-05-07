#pragma once
#include <vector>

using ull = unsigned long long int;

template <class T>
class LongVector {
public:
    ull _size;

public:
    ull size(){return _size;}
    //return the size after push_back
    virtual ull push_back(const T& v) = 0;
    virtual ull pop_back() = 0;
    
    virtual T& operator[](ull idx) = 0;
    virtual void resize(ull size) = 0;
};