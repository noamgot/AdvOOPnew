#pragma once

// code taken mainly from:
// https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class MyQueue
{
	std::queue<T> _queue;
	std::mutex _mutex;
	std::condition_variable _cv;
	bool _isAlive;

public:

	struct IsDead{}; // for handling end of tournament

	MyQueue() : _isAlive(true){}
	// block copy & move ctors and assignments
	MyQueue(const MyQueue& other) = delete;
	MyQueue& operator=(const MyQueue& other) = delete;
	MyQueue(MyQueue&& other) noexcept = delete;
	MyQueue& operator=(MyQueue&& other) noexcept = delete;

	/* return the first item of the queue and deletes it*/
	T pop();

	void push(const T& item);
	void push(T&& item);

	void kill();

	bool empty()
	{
		std::lock_guard<std::mutex> mlock(_mutex);
		return _queue.empty();
	}
	
};


