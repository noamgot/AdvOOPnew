#pragma once
#include <initializer_list>

template <class T>
class Matrix2d
{
private:

public:
	Matrix2d(std::initializer_list < std::initializer_list<T>>);
	T& groupValues(T(*func)(char));
	
};
