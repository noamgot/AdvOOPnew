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
	SafeQueue() = default;
	// block copy & move ctors and assignments
	SafeQueue(const SafeQueue& other) = delete;
	SafeQueue& operator=(const SafeQueue& other) = delete;
	SafeQueue(SafeQueue&& other) noexcept = delete;
	SafeQueue& operator=(SafeQueue&& other) noexcept = delete;

	/* returns the first item of the queue and deletes it*/
	T pop();

	void push(const T& item);
	void push(T&& item);

	
private:
	std::queue<T> _queue;
	std::mutex _mutex;
	std::condition_variable _cv;
};

template <class T>
T SafeQueue<T>::pop()
{
	std::unique_lock<std::mutex> mlock(_mutex);
	_cv.wait(mlock, [this] {return !_queue.empty(); });
	T item = std::move(_queue.front());
	_queue.pop();
	return item;

}

template <class T>
void SafeQueue<T>::push(const T& item)
{
	std::unique_lock<std::mutex> mlock(_mutex);
	_queue.push(item);
	mlock.unlock();
	_cv.notify_one();
}

template <class T>
void SafeQueue<T>::push(T&& item)
{
	std::unique_lock<std::mutex> mlock(_mutex);
	_queue.push(std::move(item));
	mlock.unlock();
	_cv.notify_one();
}

