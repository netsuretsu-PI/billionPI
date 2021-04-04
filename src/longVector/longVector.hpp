#pragma once
#include <vector>

using ull = unsigned long long int;

template <class T>
class LongVector {
    std::vector<T> ar;
    ull _size;

public:
    ull size();
};