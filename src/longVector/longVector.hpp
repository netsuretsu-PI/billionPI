#pragma once
#include <vector>

using ull = unsigned long long int;

template <class T>
class LongVector {
    static const ull PAGE_SIZE = 1; //MB
    static const ull CACHE_SIZE = 10; //pages
    ull size;

public:
    LongVector(ull size);
    ull size();
    //return the size after push_back
    ull push_back(const T& v);
    ull pop_back();
    
    T operator[](ull idx);
    T resize(ull size);

    ~LongVector(); //release resources (delete files)
};