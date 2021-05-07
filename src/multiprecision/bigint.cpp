#include "bigint.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include <cassert>
#include <complex>

namespace mp = boost::multiprecision;

using namespace std;

Fourier::Fourier(vector<cmplx> _limbs) : limbs(_limbs) {}
Fourier::Fourier(int sz) : limbs(sz) {}
BigInt Fourier::IFFT() {
    BigInt ret(limbs.size());
    FileBasedVector<cmplx> buf;
    buf = limbs;
    ifft(buf);
    for (int i = 0; limbs.size() > i; i++) {
        ret.limbs[i] = round(buf[i].real());
    }
    ret.normalize();
    return ret;
}

void Fourier::operator*=( Fourier& b) {
    assert(limbs.size() == b.limbs.size());
    for (int i = 0; limbs.size() > i; i++) {
        limbs[i] *= b.limbs[i];
    }
}

Fourier Fourier::operator*( Fourier& b)  {
    Fourier ret(limbs.size());
    assert(limbs.size() == b.limbs.size());
    for (int i = 0; limbs.size() > i; i++) {
        ret.limbs[i] = limbs[i] * b.limbs[i];
    }
    return ret;
}

BigInt::BigInt() : limbs(1) {}
BigInt::BigInt(size_t sz) : limbs(sz) {}
BigInt::BigInt(unsigned long long a) {
    if (a == 0) limbs.push_back(0);
    while (a > 0) {
        limbs.push_back(a % BASE);
        a /= BASE;
    }
}
BigInt BigInt::operator*(BigInt& b) {
    size_t n = max(limbs.size(), b.limbs.size());
    BigInt c(size_t(1 << (getFSize(n) + 1)));
    convolve(limbs, b.limbs, c.limbs);
    c.normalize();
// #ifdef DEBUG
    if (c.toCppInt() != b.toCppInt() * toCppInt()) {
        cout << c.size() << " " << limbs.size() << " " << b.limbs.size()
             << endl;
    }
// #endif
    return c;
}

BigInt BigInt::operator*(BigInt&& b) {
    size_t n = max(limbs.size(), b.limbs.size());
    BigInt c(size_t(1 << (getFSize(n) + 1)));
    convolve(limbs, b.limbs, c.limbs);
    c.normalize();
#ifdef DEBUG
    if (c.toCppInt() != b.toCppInt() * toCppInt()) {
        cout << c.size() << " " << limbs.size() << " " << b.limbs.size()
             << endl;
    }
#endif
    return c;
}
Fourier BigInt::FFT(size_t hn) {
    Fourier f(1 << (getFSize(max((ull)hn, limbs.size())) + 1));
    for (int i = 0; limbs.size() > i; i++) f.limbs[i] = limbs[i];
    fft(f.limbs);
    return f;
}
void BigInt::print() {
    long long int sum = 0, b = 1;
    for (int i = 0; limbs.size() > i; i++) {
        printf("%d ", limbs[i]);
        sum += b * limbs[i];
        b *= BASE;
    }
    printf("%lld\n", sum);
}
unsigned long long BigInt::toULL() {
    unsigned long long int sum = 0, b = 1;
    for (int i = 0; limbs.size() > i; i++) {
        sum += b * limbs[i];
        b *= BASE;
    }
    return sum;
}
double BigInt::toDouble()  {
    double sum = 0;
    for (int i = 0; limbs.size() > i; i++) {
        double cur = pow(BASE, i) * limbs[i];
        sum += cur;
    }
    return sum;
}
void Fourier::print() {
    for (int i = 0; limbs.size() > i; i++) {
        printf("%lf,%lf\n", limbs[i].real(), limbs[i].imag());
    }
}

void BigInt::normalize() {
    LIMB carry = 0;
    int msl = 0;
    for (int i = 0; limbs.size() > i; i++) {
        limbs[i] += carry;
        carry = 0;
        if (limbs[i] >= BASE) {
            carry = limbs[i] >> BASE_E;
            limbs[i] &= BASE_MASK;
        }
        if (limbs[i] != 0) msl = i;
    }
    while (carry > 0) {
        LIMB cur = carry & BASE_MASK;
        limbs.push_back(cur);
        carry >>= BASE_E;
        if (cur != 0) msl = limbs.size() - 1;
    }

    if (limbs.size() > msl * 1.5) limbs.resize(msl + 1);
}

size_t BigInt::size() { return limbs.size(); }

void BigInt::operator+=( BigInt& b) {
    if (limbs.size() < b.limbs.size()) limbs.resize(b.limbs.size());
    long long carry = 0;
    for (int i = 0; b.limbs.size() > i; i++) {
        limbs[i] += b.limbs[i] + carry;
        carry = 0;
        if (limbs[i] >= BASE) {
            carry = 1;
            limbs[i] &= BASE_MASK;
        }
    }
    for (int i = b.limbs.size(); limbs.size() > i; i++) {
        limbs[i] += carry;
        carry = 0;
        if (limbs[i] >= BASE) {
            carry = 1;
            limbs[i] &= BASE_MASK;
        }
    }
    if (carry == 1) {
        limbs.push_back(1);
    }
}

void BigInt::operator-=( BigInt& b) {
    if (limbs.size() < b.limbs.size()) limbs.resize(b.limbs.size());
    long long carry = 0;
    for (int i = 0; b.limbs.size() > i; i++) {
        if (limbs[i] < b.limbs[i] + carry) {
            limbs[i] += BASE - b.limbs[i] - carry;
            carry = 1;
        } else {
            limbs[i] -= b.limbs[i] + carry;
            carry = 0;
        }
    }
    for (int i = b.limbs.size(); limbs.size() > i; i++) {
        limbs[i] -= carry;
        carry = 0;
        if (limbs[i] < 0) {
            carry = 1;
            limbs[i] += BASE;
        }
        if (carry == 0) break;
    }
    assert(carry == 0);
}

BigInt BigInt::operator+( BigInt& b)  {
    BigInt ret((size_t)0);
    ret.limbs = limbs;
    ret += b;
    return ret;
}
BigInt BigInt::operator+( BigInt&& b)  {
    BigInt ret((size_t)0);
    ret.limbs = limbs;
    ret += b;
    return ret;
}

bool BigInt::operator>( BigInt& b)  {
    for (int i = max(limbs.size(), b.limbs.size()) - 1; 0 <= i; i--) {
        int an = limbs.size() > i ? limbs[i] : 0;
        int bn = b.limbs.size() > i ? b.limbs[i] : 0;
        if (an > bn) return true;
        if (bn > an) return false;
    }
    return false;
}

bool BigInt::operator<( BigInt& b)  {
    for (int i = max(limbs.size(), b.limbs.size()) - 1; 0 <= i; i--) {
        int an = limbs.size() > i ? limbs[i] : 0;
        int bn = b.limbs.size() > i ? b.limbs[i] : 0;
        if (bn > an) return true;
        if (an > bn) return false;
    }
    return false;
}

BigInt BigInt::operator-( BigInt& b)  {
    BigInt ret((size_t)0);
    ret.limbs = limbs;
    ret -= b;
    return ret;
}

unsigned long long int BigInt::MSL() {
    for (int i = limbs.size() - 1; 0 <= i; i--) {
        if (limbs[i] != 0) return i;
    }
    return 0;
}
unsigned long long BigInt::LSL() {
    for (int i = 0; limbs.size() > i; i++) {
        if (limbs[i] != 0) return i;
    }
    return limbs.size() - 1;
}

mp::cpp_int BigInt::toCppInt()  {
    mp::cpp_int ret = 0;
    for (int i = limbs.size() - 1; i >= 0; i--) {
        ret *= BASE;
        ret += limbs[i];
    }
    return ret;
}