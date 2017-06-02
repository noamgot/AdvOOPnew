#include "SafeQueue.h"

/*template <class T>
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
}*/