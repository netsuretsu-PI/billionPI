#pragma once
#include <../longVector.hpp>
#include <string>
#include <atomic>
#include <map>
#include <queue>
#include <vector>
#include <set>
#include <algorithm>
using std::string;
using std::atomic;
using std::set;
using std::map;
using std::priority_queue;
using std::vector;
using std::greater;

using TimeStamp = time_t;

// NOT THREAD SAFE
template <class T>
class FileBasedVector : public LongVector<T> {
    static const ull PAGE_SIZE = 1; //num of elements
    static const ull CACHE_SIZE = 2; //num of pages
    static const ull BYTE_OF_PAGE = PAGE_SIZE * sizeof(T);

struct PageInfo {
    void* address;
    TimeStamp lastUsedTime;
    ull pageIdx;
    T& get(ull idx)const{
        return *(T*)(address + (idx - getStartIndex()) * sizeof(T));
    }
    ull getStartIndex()const{return pageIdx * PAGE_SIZE;}
    bool operator>(const PageInfo& b) const {
        return lastUsedTime > b.lastUsedTime;
    }
    bool operator<(const PageInfo& b) const {
        return lastUsedTime < b.lastUsedTime;
    }
};

    static atomic<int> globalVectorCounter = 0;
    
    string fileName;
    int fd = -1;
    ull reservedPage = 0;

    map<ull, PageInfo> cachedPages;
    set<PageInfo> lastUsedPagePQ;
public:
    FileBasedVector(vector<T> v);
    FileBasedVector(ull size);

    ull push_back(const T& v);
    ull pop_back();
    T& operator[](ull idx);
    void resize(ull _size);
    ~FileBasedVector(); //release resources (delete files)

private:
    PageInfo cache(ull pageIdx);
    void removeOldestCache();
    void removeCache(PageInfo page);
    void resizeInPage(ull num);
    void* getPageAddress(ull pageIdx);

    ull getSizeInByte();
    ull getSizeInPage();
    ull getReservedPage();
};