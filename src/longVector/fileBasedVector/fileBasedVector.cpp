#include "fileBasedVector.hpp"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>

template <class T>
FileBasedVector<T>::FileBasedVector(ull _size){
    fileName = "vector_"std::to_string(++globalVectorCounter);
    fd = open(fileName.c_str(), O_CREAT);
    if(fd == -1) throw std::runtime_error("failed to open file");

    addPage((_size + PAGE_SIZE - 1) / PAGE_SIZE);
}

template <class T>
void FileBasedVector<T>::resizeInPage(ull num){
    if(reservedPage >= num) return;
    if(ftruncate(fd, num * BYTE_OF_PAGE ) ) != 0)
        throw std::runtime_error("failed to truncate file");
    reservedPage++;
}

template <class T>
ull FileBasedVector<T>::getSizeInByte(){
    return _size * sizeof(T);
}
template <class T>
ull FileBasedVector<T>::getSizeInPage(){
    return (_size + PAGE_SIZE - 1) / PAGE_SIZE;
}
template <class T>
ull FileBasedVector<T>::getReservedPage(){
    return reservedPage;
}

template <class T>
ull FileBasedVector<T>::push_back(const T& v){
    _size++;
    resizeInPage(_size / PAGE_SIZE);
    operator[](_size - 1) = v;
}
template <class T>
ull FileBasedVector<T>::pop_back(){
    throw std::runtime_error("not implemented!!!");
}
    
template <class T>
T& FileBasedVector<T>::operator[](ull idx){
    if(idx >= _size) throw std::out_of_range();
    ull pageIdx = idx / PAGE_SIZE;
    PageInfo page = cache(pageIdx);
    return page.get(idx);
}

template <class T>
FileBasedVector<T>::PageInfo FileBasedVector<T>::cache(ull pageIdx){
    if(cachedPages.find(pageIdx) == cachedPages.end()) return cachedPages[pageIdx];

    if(cachedPages.size() == CACHE_SIZE)
}

template <class T>
void FileBasedVector<T>::removeOldestCache(){
    if(lastUsedPagePQ.size() == 0) return;
    removeCache(*lastUsedPagePQ.begin());
}

template <class T>
void FileBasedVector<T>::removeCache(PageInfo page){
    lastUsedPagePQ.erase(lastUsedPagePQ.find(page));
    cachedPages.erase(page.pageIdx);
}


template <class T>
void FileBasedVector<T>::resize(ull _size){
    resizeInPage(size / PAGE_SIZE);
    _size = _size;
}

template <class T>
FileBasedVector<T>::~FileBasedVector(){
    for(auto page : cachedPages) {
        if(munmap(page.first, PAGE_SIZE * sizeof(T)) == -1)
            throw std::runtime_error("failed to munmap");
    }
    close(fd);
    unlink(fileName.c_str());
}