#include "MyQueue.h"

template <typename T>
T MyQueue<T>::pop()
{
	std::unique_lock<std::mutex> mlock(_mutex);
	_cv.wait(mlock, [this] {return !(_queue.empty() && _isAlive); });
	// if the queue is not empty, we still want to consume the next game (even if _isAlive = false)
	if(!_queue.empty())
	{
		T item = _queue.front();
		_queue.pop();
		return item;
		
	}
	// in this case the queue is empty - we notify all waiting threads (and especially the one that waits on kill())
	mlock.unlock();
	_cv.notify_all();
	throw IsDead();	
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

template <typename T>
void MyQueue<T>::kill()
{
	std::unique_lock<std::mutex> mlock(_mutex);
	_isAlive = false;
	mlock.unlock();
	_cv.notify_all();
	// wait until all elements are consumed
	_cv.wait(mlock, [this] {return _queue.empty(); });
}

