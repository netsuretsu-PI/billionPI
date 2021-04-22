#pragma once
#include <vector>

using ull = unsigned long long int;

template <class T>
class LongVector {
    ull _size;

public:
    LongVector(ull size);
    ull size();
    //return the size after push_back
    virtual ull push_back(const T& v);
    virtual ull pop_back();
    
    virtual T operator[](ull idx);
    virtual void resize(ull size);

    ~LongVector(); //release resources (delete files)
};