#include "fileBasedVector.hpp"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>

#include <string.h>
#include <errno.h>

using std::move;
using std::make_pair;

atomic<int> globalVectorCounter(0);
template <class T>
FileBasedVector<T>::FileBasedVector(vector<T>& v):FileBasedVector(v.size()){
    for(int i = 0; v.size() > i; i++)
        operator[](i) = v[i];
}
template <class T>
FileBasedVector<T>::FileBasedVector():FileBasedVector(0){}
template <class T>
FileBasedVector<T>::FileBasedVector(ull _size){
    if(BYTE_OF_PAGE % sysconf(_SC_PAGE_SIZE) != 0){
        throw std::runtime_error("invalid PAGE_SIZE: sysconf(_SC_PAGE_SIZE) is " + std::to_string(sysconf(_SC_PAGE_SIZE)));
    }

    fileName = "vecData/vector_" + std::to_string(++globalVectorCounter);
    fd = open(fileName.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRWXO);
    if(fd == -1) throw std::runtime_error(strerror(errno));

    resize(_size);
}

template <class T>
void FileBasedVector<T>::reserveInPage(ull num){
    if(reservedPage >= num) return;
    if(ftruncate(fd, num * BYTE_OF_PAGE ) != 0)
        throw std::runtime_error("failed to truncate file");
    reservedPage = num;
}

template <class T>
ull FileBasedVector<T>::getSizeInByte(){
    return this->_size * sizeof(T);
}
template <class T>
ull FileBasedVector<T>::getSizeInPage(){
    return (this->_size + PAGE_SIZE - 1) / PAGE_SIZE;
}
template <class T>
ull FileBasedVector<T>::getReservedPage(){
    return reservedPage;
}

template <class T>
ull FileBasedVector<T>::push_back(const T& v){
    resize(this->_size + 1);
    operator[](this->_size - 1) = v;
    return this->_size;
}
template <class T>
ull FileBasedVector<T>::pop_back(){
    if(this->_size == 0) throw std::runtime_error("there is no element");
    resize(this->_size - 1);
    return this->_size;
}

template <class T>
T& FileBasedVector<T>::operator[](ull idx){
    if(idx >= this->_size) throw std::out_of_range("out of range: idx:" + std::to_string(idx));
    ull pageIdx = idx / PAGE_SIZE;
    PageInfoPtr page = cache(pageIdx);
    touchPage(page);
    return page->get(idx);
}

template <class T>
void FileBasedVector<T>::touchPage(PageInfoPtr page){
    auto itr = lastUsedPagePQ.find(make_pair(page->lastUsedTime, page));
    if(itr != lastUsedPagePQ.end())
        lastUsedPagePQ.erase(itr);
    page->lastUsedTime = getTime();
    lastUsedPagePQ.insert(make_pair(page->lastUsedTime, page));
}

template <class T>
TimeStamp FileBasedVector<T>::getTime(){
    return time(NULL);
}


template <class T>
typename FileBasedVector<T>::PageInfoPtr FileBasedVector<T>::cache(ull pageIdx){
    if(cachedPages.find(pageIdx) != cachedPages.end()) return cachedPages[pageIdx];

    if(cachedPages.size() == CACHE_SIZE) removeOldestCache();
    PageInfoPtr page(new PageInfo(
        (T*) mmap(NULL, BYTE_OF_PAGE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BYTE_OF_PAGE * pageIdx),
        pageIdx
    ));
    
    if((ull)((void*)page->address) == -1){
        throw std::runtime_error(strerror(errno));
    }
    cachedPages[pageIdx] = page;
    return page;
}

template <class T>
void FileBasedVector<T>::removeOldestCache(){
    if(lastUsedPagePQ.size() == 0) return;
    removeCache(lastUsedPagePQ.begin()->second);
}

template <class T>
void FileBasedVector<T>::removeCache(PageInfoPtr page){
    if(munmap(page->address, PAGE_SIZE * sizeof(T)) == -1)
        throw std::runtime_error("failed to munmap");
    lastUsedPagePQ.erase(make_pair(page->lastUsedTime,page));
    cachedPages.erase(page->pageIdx);
}

template <class T>
void FileBasedVector<T>::resize(ull _size){
    //reserve
    reserveInPage((_size + PAGE_SIZE -1 ) / PAGE_SIZE);
    this->_size = _size;
}

template <class T>
void FileBasedVector<T>::freeResources(){
    auto cp = cachedPages;
    for(auto page : cp) 
        removeCache(page.second);
    
    close(fd);
    unlink(fileName.c_str());
}

template <class T>
FileBasedVector<T>::~FileBasedVector(){
    freeResources();
}

template <class T>
FileBasedVector<T>::FileBasedVector(FileBasedVector<T>&& src){
    fileName = src.fileName;
    fd = src.fd;
    reservedPage = src.reservedPage;
    cachedPages = std::move(src.cachedPages);
    lastUsedPagePQ = std::move(src.lastUsedPagePQ);
}

template <class T>
FileBasedVector<T>&& FileBasedVector<T>::copy(){
    FileBasedVector<T> ret(this->_size);
    for(int i = 0; this->_size > i; i++){
        ret[i] = operator[](i);
    }
    return std::move(ret);
}

template <class T>
FileBasedVector<T>& FileBasedVector<T>::operator=(FileBasedVector<T>&& r){
    freeResources();

    fileName = r.fileName;
    fd = r.fd;
    reservedPage = r.reservedPage;
    cachedPages = r.cachedPages;
    lastUsedPagePQ = r.lastUsedPagePQ;
}