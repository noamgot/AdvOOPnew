#pragma once

// code taken mainly from:
// https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <class T>
class SafeQueue
{
public:

	// this struct is for handling end of tournament - it gets thrown when a thread gets to an empty queue
	// after the games production is done
	//struct IsDead{}; 

	SafeQueue() = default;	

	//SafeQueue() : _isAlive(true){}
	// block copy & move ctors and assignments
	SafeQueue(const SafeQueue& other) = delete;
	SafeQueue& operator=(const SafeQueue& other) = delete;
	SafeQueue(SafeQueue&& other) noexcept = delete;
	SafeQueue& operator=(SafeQueue&& other) noexcept = delete;

	/* returns the first item of the queue and deletes it*/
	T pop();

	void push(const T& item);
	void push(T&& item);

	//void kill();

/*	bool empty()
	{
		std::lock_guard<std::mutex> mlock(_mutex);
		return _queue.empty();
	}*/
	
private:
	std::queue<T> _queue;
	std::mutex _mutex;
	std::condition_variable _cv;
//	bool _isAlive;
};


