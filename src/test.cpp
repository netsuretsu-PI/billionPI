#include "./longVector/fileBasedVector/fileBasedVector.hpp"
#include <cassert>
#include <iostream>
#include "./multiprecision/multiprec.hpp"
#include "./multiprecision/bigint.hpp"
#include "./multiprecision/bigfloat.hpp"
using std::cout;
using std::endl;

void simpleWriteTest(){
    FileBasedVector<int> v(100000000);
    cout << "constructed" << endl;
    for(ull i = 0; 100000000LL > i; i++)
        v[i] = i;
    cout << "wrote" << endl;
    for(ull i = 0; 100000000LL > i; i++)
        assert(v[i] == i);
}
void fftTest(){
    FileBasedVector<ull> a(128);
    FileBasedVector<ull> b(128);
    FileBasedVector<ull> c(256);
    cout << "constructed" << endl;
    a[0] = 1, a[1] = 2, a[2] = 3;
    b[0] = 2, b[1] = 2, b[2] = 4;

    convolve(a, b, c);
    assert(c[0] == 2 && c[1] == 6 && c[2] == 14 && c[3] == 14 && c[4] == 12);
}
void bigIntTest() {
    BigInt a(10000ULL), b(12312ULL);
    cout << (a + b).toCppInt() << endl;
    cout << (a * b).toCppInt() << endl;
    cout << (a * b * a * b * a * b * a * b * a * b * a * b * a).toCppInt() << endl;
}

void bigFloatTest() {
    BigFloat a(0.242), b(0.3331), c(7.0);

    cout << c.toDouble() << endl;
    a.print();
    cout << a.toDouble() << endl;
    cout << (a + b).toDouble() << endl;
    cout << (a * b).toDouble() << endl;
    cout << c.toDouble() << endl;
    cout << (a * a * c * b).toDouble() << endl;
    cout << c.toDouble() << endl;
    cout << c.reciprocal(100).toDouble() << endl;
    cout << invsqrt(c, 100).toDouble() << endl;
    // cout << (a * b).toDouble() << endl;
    // cout << (a * b * a * b * a * b * a * b * a * b * a * b * a).toDouble() << endl;
}
void fileBasedVectorTest(){
    // simpleWriteTest();
    // fftTest();
    // bigIntTest();
    bigFloatTest();
}

int main(){
    fileBasedVectorTest();
    return 0;
}