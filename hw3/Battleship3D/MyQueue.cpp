#include "MyQueue.h"

template <typename T>
T MyQueue<T>::pop()
{
	std::unique_lock<std::mutex> mlock(_mutex);
	while (_queue.empty())
	{
		_cv.wait(mlock);
	}
	T item = _queue.front();
	_queue.pop();
	return item;
}

template <typename T>
void MyQueue<T>::push(const T& item)
{
	std::unique_lock<std::mutex> mlock(_mutex);
	_queue.push(item);
	mlock.unlock();
	_cv.notify_one();
}

template <typename T>
void MyQueue<T>::push(T&& item)
{
	std::unique_lock<std::mutex> mlock(_mutex);
	_queue.push(std::move(item));
	mlock.unlock();
	_cv.notify_one();
}

