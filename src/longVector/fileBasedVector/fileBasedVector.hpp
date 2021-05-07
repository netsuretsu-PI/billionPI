#pragma once
#include "../longVector.hpp"
#include <string>
#include <atomic>
#include <map>
#include <queue>
#include <vector>
#include <set>
#include <algorithm>
#include <memory>
#include <utility>
#include <unistd.h>
#include <complex>
using std::shared_ptr;
using std::string;
using std::atomic;
using std::set;
using std::map;
using std::priority_queue;
using std::vector;
using std::greater;
using std::pair;

using TimeStamp = time_t;

extern atomic<int> globalVectorCounter;

// NOT THREAD SAFE
template <class T>
class FileBasedVector : public virtual LongVector<T> {
    static const ull PAGE_SIZE = 1024; //num of elements
    static const ull CACHE_SIZE = 2; //num of pages
    static const ull BYTE_OF_PAGE = PAGE_SIZE * sizeof(T);

struct PageInfo {
    T* address;
    TimeStamp lastUsedTime = -1;
    ull pageIdx;
    PageInfo(T* address, ull pageIdx){
        this->address = address;
        this->pageIdx = pageIdx;
    }
    T& get(ull idx)const{
        return address[idx - pageIdx * PAGE_SIZE];
    }
    bool operator>(const PageInfo& b) const {
        return lastUsedTime > b.lastUsedTime;
    }
    bool operator<(const PageInfo& b) const {
        return lastUsedTime < b.lastUsedTime;
    }
};
    using PageInfoPtr = shared_ptr<PageInfo>;
    
    string fileName;
    int fd = -1;
    ull reservedPage = 0;

    map<ull, PageInfoPtr> cachedPages;
    set<pair<TimeStamp, PageInfoPtr>> lastUsedPagePQ;
public:
    FileBasedVector(vector<T>& v);
    FileBasedVector(ull size);
    FileBasedVector();

    ull push_back(const T& v);
    ull pop_back();
    T& operator[](ull idx);
    void resize(ull _size);
    ~FileBasedVector(); //release resources (delete files)

    FileBasedVector(FileBasedVector<T>&& src) noexcept;
    FileBasedVector& operator=(FileBasedVector&& r) noexcept;

    void operator=(FileBasedVector<T>& src);
private:
    PageInfoPtr cache(ull pageIdx);
    void removeOldestCache();
    void removeCache(PageInfoPtr page);
    void reserveInPage(ull num);
    void* getPageAddress(ull pageIdx);
    void touchPage(PageInfoPtr page);
    void freeResources() noexcept;

    ull getSizeInByte();
    ull getSizeInPage();
    ull getReservedPage();

    TimeStamp getTime();

};

template class FileBasedVector<int>;
template class FileBasedVector<unsigned int>;
template class FileBasedVector<long long int>;
template class FileBasedVector<unsigned long long int>;
template class FileBasedVector<double>;
template class FileBasedVector<std::complex<double>>;
template class FileBasedVector<std::complex<long double>>;