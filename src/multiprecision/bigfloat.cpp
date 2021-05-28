#include "bigfloat.hpp"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <thread>

#include "bigint.hpp"
using namespace std;

BigFloat::BigFloat(BigInt a, long long int _exponent, bool _sign) {
    fraction = a;
    exponent = _exponent;
    sign = _sign;
}

unsigned long long int extractBit(double v, int st, int en) {
    unsigned long long int uv = *(unsigned long long int*)(&v);
    return (uv << (64 - en)) >> (64 - en + st);
}

BigFloat::BigFloat(double a) {
    sign = extractBit(a, 63, 64);
    int origexp = (extractBit(a, 52, 63) - 1023 - 52);
    int sa = (origexp % BASE_E + BASE_E) % BASE_E;
    int nexp = origexp - sa;
    exponent = nexp / BASE_E;
    fraction = BigInt((extractBit(a, 0, 52) | (1LL << 52)) << sa);
}

BigFloat getInitialR(BigFloat& b) {
    b.shrink();
    unsigned long long int v = 0;
    unsigned long long int base = 1;
    int st = max(0, (int)b.fraction.size() - 4);
    for (int i = st; i < b.fraction.size(); i++) {
        v += base * b.fraction.limbs[i];
        base *= BASE;
    }

    BigFloat ret(1.0 / v);
    ret.exponent += -(b.exponent + st);
    ret.sign = b.sign;
    return ret;
}

BigFloat BigFloat::reciprocal(unsigned long long int digit) {
    cout << "reciprocal" << endl;
    shrink();
    long long int ordig = fraction.size();

    BigFloat one(BigInt(1ULL));
    // cout << "reci : " << init << endl;
    // BigFloat r(BigInt(1ULL), exptwo, sign);
    BigFloat r = getInitialR(*this);
    BigFloat prod = r * (*this), delta = one - prod;
    long long prec = -(delta.exponent + (long long)delta.fraction.MSL()) - 1;
    while (max(0ll, prec) < digit) {
        cout << "prec:" << prec << endl;
        cout << "delta:" << delta.toDouble() << endl;
        cout << "r:" << r.toDouble() << endl;
        cout << delta.sign << endl;

        BigFloat upd = r * delta;
        r = r + upd;
        prod = r * (*this);
        delta = one - prod;
        prec = -(delta.exponent + (long long)delta.fraction.MSL()) - 1;

        r.changeExponent(min(-ordig - prec * 2, -prec * 2));
        delta.changeExponent(min(-ordig - prec * 2, -prec * 2));
    }
    cout << "prec:" << prec << endl;
    return r;
}

BigFloat BigFloat::operator*(const BigFloat& b) {
    BigFloat ret(b.fraction * this->fraction, b.exponent + this->exponent,
                 sign ^ b.sign);
    ret.shrink();
    return ret;
}

BigFloat BigFloat::operator+(BigFloat& b) {
    long long int nexponent = min(exponent, b.exponent);
    changeExponent(nexponent);
    b.changeExponent(nexponent);

    BigFloat ret(0.0);
    if (b.sign == sign) {
        ret = BigFloat(fraction + b.fraction, nexponent, sign);
    } else {
        if (b.fraction < fraction) {
            ret = BigFloat(fraction - b.fraction, nexponent, !b.sign);
        } else {
            ret = BigFloat(b.fraction - fraction, nexponent, b.sign);
        }
    }
    ret.shrink();
    return ret;
}

BigFloat BigFloat::operator-(BigFloat& b) {
    b.negate();
    BigFloat ret = (*this) + b;
    b.negate();
    return ret;
}

void BigFloat::changeExponent(long long int nexponent) {
    int diff = exponent - nexponent;
    exponent = nexponent;
    if (diff > 0) {
        fraction.limbs.insert(fraction.limbs.begin(), diff, 0);
    } else if (diff < 0) {
        diff = max(0, -diff);
        if (fraction.limbs.size() <= diff) {
            fraction.limbs = vector<LIMB>(1);
        } else
            fraction.limbs =
                vector<LIMB>(fraction.limbs.begin() + diff,
                             fraction.limbs.begin() +
                                 max({(unsigned long long)diff,
                                      (unsigned long long)fraction.limbs.size(),
                                      fraction.MSL() + 1}));
    }
}

void BigFloat::negate() { sign = !sign; }

void BigFloat::print() {
    cout << "sign : " << sign << endl;
    cout << "exponen : " << exponent << endl;
    cout << "topexponen : " << exponent + fraction.limbs.size() + 1 << endl;
    fraction.print();
}

unsigned long long int floor(BigFloat& x) {
    assert(x.exponent + (int)x.fraction.limbs.size() <= 1);
    if ((x.exponent) + (int)x.fraction.limbs.size() == 1) {
        return x.fraction.limbs.back();
    } else {
        return 0ULL;
    }
}

BigFloat pow10(int exp) {
    if (exp == 0) {
        return BigFloat(1.0);
    }
    auto prev = pow10(exp / 2);
    BigFloat ret(0.0);
    if (exp % 2 == 0) {
        ret = prev * prev;
    } else {
        ret = (prev * prev * BigFloat(10.0));
    }
    ret.shrink();
    return ret;
}

void dumpBigFloatPrimitive(BigFloat& x, ostream& ost,
                           unsigned long long int prec) {
    BigFloat ten(10.0);
    ost << floor(x) << ".";
    if (floor(x) != 0) {
        x.fraction.limbs.pop_back();
    }
    for (unsigned long long int i = 0; i < prec; i++) {
        x = x * ten;
        ost << floor(x);
        if (floor(x) != 0) {
            x.fraction.limbs.pop_back();
        }
    }
    ost << endl;
}
std::mutex mtx;
std::condition_variable cv;
void dumpBigFloat(BigFloat x, ostream& ost, unsigned long long int prec,
                  int depth, int id, int& nextDumpId) {
    if (depth == 0) {
        ost << floor(x) << ".";
        if (floor(x) != 0) {
            x.fraction.limbs.pop_back();
        }
    }
    if (depth == 5) {
        BigFloat ten(10.0);
        string str = "";
        for (unsigned long long int i = 0; str.size() < prec; i++) {
            x = x * ten;
            str += to_string(floor(x));
            if (floor(x) != 0) {
                x.fraction.limbs.pop_back();
            }
        }
        while (str.size() != prec) {
            str.pop_back();
        }
        {
            std::unique_lock<std::mutex> uniq_lk(mtx);
            cv.wait(uniq_lk, [&] { return id == nextDumpId; });

            ost << str;
            if ((id == (1 << depth) - 1)) {
                ost << endl;
            }
            nextDumpId++;
        }
        cv.notify_all();

    } else {
        int firstPrec = prec / 2;
        int secondPrec = prec - firstPrec;

        auto second = x;
        int diff = (prec - firstPrec) / BASE_10;
        x.fraction.limbs = vector<LIMB>(x.fraction.limbs.begin() + diff,
                                        x.fraction.limbs.end());
        x.exponent += diff;
        x.shrink();

        auto padding = pow10(firstPrec);
        second = second * padding;
        assert(second.exponent < 0);
        while (second.fraction.limbs.size() != -second.exponent) {
            second.fraction.limbs.pop_back();
        }
        second.shrink();

        thread th1(dumpBigFloat, x, std::ref(ost), firstPrec, depth + 1, id * 2,
                   std::ref(nextDumpId));
        thread th2(dumpBigFloat, second, std::ref(ost), secondPrec, depth + 1,
                   id * 2 + 1, std::ref(nextDumpId));

        th1.join();
        th2.join();
    }
}

double BigFloat::toDouble() {
    double ret = 0;
    // double base = pow(BASE, exponent + fraction.limbs.size() - 1);
    for (int i = 0; fraction.limbs.size() > i; i++) {
        if (fraction.limbs[i] == 0) continue;
        double base = pow(BASE, exponent + i);
        ret += fraction.limbs[i] * base;
        // base /= BASE;
    }
    return ret * (sign * (-2) + 1);
}

BigFloat getInitialI(BigFloat& b) {
    b.shrink();
    unsigned long long int v = 0;
    unsigned long long int base = 1;
    int st = max(0, (int)b.fraction.size() - 4);

    for (int i = st; i < b.fraction.size(); i++) {
        v += base * b.fraction.limbs[i];
        base *= BASE;
    }

    BigFloat ret(1.0 / sqrt(v));
    ret.exponent += -(b.exponent + st + 1) / 2;
    if ((b.exponent + st) % 2) {
        ret = ret * BigFloat(BASE_SQRT);
    }
    ret.sign = b.sign;
    return ret;
}

BigFloat invsqrt(BigFloat& b, unsigned long long int digit) {
    cout << "invsqrt" << endl;
    BigFloat one(BigInt(1ULL));
    BigFloat invtwo = BigFloat(0.5);
    BigFloat r = getInitialI(b);
    BigFloat prod = r * r * b;
    BigFloat delta = one - prod;
    long long prec = -(delta.exponent + (long long)delta.fraction.MSL()) - 1;
    while (prec < digit) {
        cout << "prec:" << prec << endl;

        BigFloat upd = r * delta * invtwo;
        r = r + upd;
        prod = r * r * b;
        delta = one - prod;
        prec = -(delta.exponent + (long long)delta.fraction.MSL()) - 1;

        r.changeExponent(-prec * 2);
        delta.changeExponent(-prec * 2);
    }
    cout << "prec:" << prec << endl;
    return r;
}

void BigFloat::shrink() {
    int lsl = fraction.LSL(), msl = fraction.MSL();
    if (lsl > msl) {
        exponent = 0;
        fraction.limbs = vector<LIMB>(1);
        return;
    }
    if (lsl == 0 && msl == fraction.limbs.size() - 1) return;
    exponent += lsl;
    fraction.limbs = vector<LIMB>(fraction.limbs.begin() + lsl,
                                  fraction.limbs.begin() + msl + 1);
}
