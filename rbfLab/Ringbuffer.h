




#pragma once
#include <iostream>
#include <condition_variable>
#include <thread>
#include <mutex>
template<typename T>
class Ringbuffer
{
private:
	T* buffer;
	int capacity;
	int inn;
	int out;
	std::mutex wLock;
public:
	Ringbuffer(int capacity);
	~Ringbuffer();
	void write(T value);
	T read();
	int getInn()const;
	int getOut()const;
	bool checkForWriteLock()const;
	bool checkForReadLock()const;
};


//constructor
template<typename T>
inline Ringbuffer<T>::Ringbuffer(int capacity):capacity(capacity){//setter this->capacity til å være capacity
	buffer = new T[capacity]; //dynamisk array med størrelse capacity
	inn = 0;
	out = 0;
}

//destructor
template<typename T>
inline Ringbuffer<T>::~Ringbuffer(){
	delete[] buffer;
}
wLock.lock(); //bare en thread får lov til å bruke write funksjonen av gangen

wLock.unlock();


//behandler data som skal inn i buffer
template<typename T>
inline void Ringbuffer<T>::write(T value){
	buffer[inn] = value;
	inn = (inn + 1) % capacity; //går fra 0 til (capacity -1) til 0 igjen, (altså i en "ring")
}

//leser av data fra buffer
template<typename T>
inline T Ringbuffer<T>::read(){
	T val = buffer[out];
	out = (out + 1) % capacity;
	return val;
}

template<typename T>
inline int Ringbuffer<T>::getInn() const{return inn;}

template<typename T>
inline int Ringbuffer<T>::getOut() const{return out;}

template<typename T>
inline bool Ringbuffer<T>::checkForWriteLock() const{
	return (inn + 1) % capacity == out;
}

template<typename T>
inline bool Ringbuffer<T>::checkForReadLock() const{
	return out == inn;
}



