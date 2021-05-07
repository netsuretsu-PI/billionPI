#pragma once
#include <complex>
#include "../longVector/fileBasedVector/fileBasedVector.hpp"
using std::vector;
using cmplx = std::complex<long double>;

const int BASE_E = 10;
const int BASE_SQRT = 1 << (BASE_E / 2);
const int BASE = 1 << BASE_E;
const int BASE_MASK = BASE - 1;

int getFSize(int n);
template<class VEC = FileBasedVector<cmplx>>
void fft(VEC& A);
template<class VEC = FileBasedVector<cmplx>>
void ifft(VEC& ar);
template <class VEC = FileBasedVector<ull>, class CMPLXVEC = FileBasedVector<cmplx>>
void convolve(VEC& _A, VEC& _B, VEC& C);
