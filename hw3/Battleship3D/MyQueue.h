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

public:
	/* return the first item of the queue and deletes it*/
	T pop();

	void push(const T& item);
	void push(T&& item);

	bool empty() { return _queue.empty(); }
	
};


