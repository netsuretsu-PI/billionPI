#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <complex>
#include <vector>

#include "multiprec.hpp"

namespace mp = boost::multiprecision;
using std::vector;

typedef unsigned long long LIMB;

class BigInt;

class Fourier {
public:
    vector<cmplx> limbs;
    Fourier(vector<cmplx> _limbs);
    Fourier(int sz);
    void operator*=(Fourier& b);
    Fourier operator*(Fourier& b);
    BigInt IFFT();
    void print();
};

class BigInt {
public:
    void normalize();
    vector<LIMB> limbs;

    BigInt();
    BigInt(size_t sz);
    BigInt(unsigned long long a);
    BigInt operator*( BigInt& b) ;
    void operator+=( BigInt& b);
    void operator-=( BigInt& b);
    BigInt operator+( BigInt& b) ;
    BigInt operator-( BigInt& b) ;
    bool operator>( BigInt& b) ;
    bool operator<( BigInt& b) ;
    Fourier FFT(size_t hn);
    size_t size();
    void print();
    unsigned long long int toULL();
    double toDouble() ;
    mp::cpp_int toCppInt() ;

    unsigned long long int MSL();
    unsigned long long int LSL();
};