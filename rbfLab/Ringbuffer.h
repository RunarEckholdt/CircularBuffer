#pragma once
#include <iostream>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <vector>

namespace rB{
	template<typename T>
	class Ringbuffer{
	public:
		Ringbuffer(int capacity = 10);
		~Ringbuffer();
		void write(T value);
		void write(T arr[], int begin, int end);
		void write(std::vector<T> vec, int begin, int end);
		void write(std::string, int lenght);
		T read();
	private:
		T* buffer;
		int capacity;
		int writeIndex;
		int readIndex;
		std::mutex wMutex;
		std::mutex rMutex;
		std::condition_variable wCv;
		std::condition_variable rCv;
		int nextIndex(int i)const;
		bool checkForWriteLock()const;
		bool checkForReadLock()const;

	};


	template<typename T>
	std::ostream& operator<<(std::ostream& os, Ringbuffer<T>& rb);


	//constructor
	template<typename T>
	inline Ringbuffer<T>::Ringbuffer(int capacity):
		capacity(capacity)//setter this->capacity til å være capacity
		, writeIndex(0)
		, readIndex(0){
		buffer = new T[capacity]; //dynamisk allokert array med størrelse capacity
	}

	//destructor
	template<typename T>
	inline Ringbuffer<T>::~Ringbuffer(){
		delete[] buffer;
	}




	//behandler data som skal inn i buffer
	template<typename T>
	void Ringbuffer<T>::write(T value){
		std::unique_lock<std::mutex> writeLock(wMutex);
		while(checkForWriteLock())//hvis nexte writeIndex index er readIndex
			wCv.wait(writeLock);//stiller seg i kø for å få skrive
		buffer[writeIndex] = value;
		writeIndex = nextIndex(writeIndex);
		rCv.notify_one(); //vekker en neste read tråden i køen til live
		writeLock.unlock();
	}

	template<typename T>
	inline void Ringbuffer<T>::write(T arr[], int begin, int end){
		for(int i = begin; i < end + 1; i++){
			write(arr[i]);
		}
	}

	template<typename T>
	inline void Ringbuffer<T>::write(std::vector<T> vec, int begin, int end){
		for(int i = begin; i < end + 1; i++){
			write(vec[i]);
		}
	}

	template<typename T>
	inline void Ringbuffer<T>::write(std::string s, int lenght){
		for(int i = 0; i < lenght; i++){
			write(s[i]);
		}
	}


	//leser av data fra buffer
	template<typename T>
	T Ringbuffer<T>::read(){
		std::unique_lock<std::mutex> readLock(rMutex);
		while(checkForReadLock())
			rCv.wait(readLock);
		T val = buffer[readIndex];
		readIndex = nextIndex(readIndex);
		wCv.notify_one(); //vekker neste write tråden i køen til live
		readLock.unlock();
		return val;
	}



	template<typename T>
	inline int Ringbuffer<T>::nextIndex(int i)const{
		return (i + 1) % capacity; //går fra 0 til (capacity -1) til 0 igjen, (altså i en "ring")
	}

	template<typename T>
	inline bool Ringbuffer<T>::checkForWriteLock() const{
		return nextIndex(writeIndex) == readIndex;
	}

	template<typename T>
	inline bool Ringbuffer<T>::checkForReadLock() const{
		return readIndex == writeIndex;
	}


	template<typename T>
	std::ostream& operator<<(std::ostream& os, Ringbuffer<T>& rb){
		return os << rb->read();
	}
};




