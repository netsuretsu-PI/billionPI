#include "./longVector/fileBasedVector/fileBasedVector.hpp"
#include <cassert>
#include <iostream>
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
    
}
void fileBasedVectorTest(){
    simpleWriteTest();
    fftTest();
}

int main(){
    fileBasedVectorTest();
    return 0;
}