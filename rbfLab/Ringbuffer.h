#pragma once
#include <iostream>
#include <condition_variable>
#include <thread>
#include <mutex>

using namespace std;

template<typename T>
class Ringbuffer
{
public:
	Ringbuffer(int capacity);
	~Ringbuffer();
	void write(T value);
	T read();
private:
	T* buffer;
	int capacity;
	int inn = 0;
	int out = 0;
	mutex wMutex;
	mutex rMutex;
	condition_variable wCv;
	condition_variable rCv;
	
	bool checkForWriteLock()const;
	bool checkForReadLock()const;
};


//constructor
template<typename T>
inline Ringbuffer<T>::Ringbuffer(int capacity):capacity(capacity){//setter this->capacity til å være capacity
	buffer = new T[capacity]; //dynamisk array med størrelse capacity
}

//destructor
template<typename T>
inline Ringbuffer<T>::~Ringbuffer(){
	delete[] buffer;
}




//behandler data som skal inn i buffer
template<typename T>
inline void Ringbuffer<T>::write(T value){
	unique_lock<mutex> writeLock(wMutex);
	while (checkForWriteLock())
		wCv.wait(writeLock);
	buffer[inn] = value;
	inn = (inn + 1) % capacity; //går fra 0 til (capacity -1) til 0 igjen, (altså i en "ring")
	rCv.notify_one(); 
	writeLock.unlock();
}

//leser av data fra buffer
template<typename T>
inline T Ringbuffer<T>::read(){
	unique_lock<mutex> readLock(rMutex);
	while (checkForReadLock())
		rCv.wait(readLock);
	T val = buffer[out];
	out = (out + 1) % capacity;
	wCv.notify_one();
	readLock.unlock();
	return val;
}


template<typename T>
inline bool Ringbuffer<T>::checkForWriteLock() const{
	return (inn + 1) % capacity == out;
}

template<typename T>
inline bool Ringbuffer<T>::checkForReadLock() const{
	return out == inn;
}



