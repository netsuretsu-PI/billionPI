#pragma oncce
#include <vector>
#include "../longVector.hpp"
#include <complex>
using std::vector;

template<class T>
class WrappedVector : public LongVector<T> {
    vector<T> vec;
public:
    WrappedVector(ull _size);
    ull push_back(const T& v);
    ull pop_back();
    T& operator[](ull idx);
    void resize(ull size);
};

template class WrappedVector<int>;
template class WrappedVector<unsigned int>;
template class WrappedVector<long long int>;
template class WrappedVector<unsigned long long int>;
template class WrappedVector<double>;
template class WrappedVector<std::complex<double>>;