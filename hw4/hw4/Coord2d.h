#pragma once
#include <initializer_list>

class Coord2d
{
	int arr[2];

	Coord2d(int y, int x)
	{
		arr[0] = y;
		arr[1] = x;
	}

	class iterator
	{
		int* pos;
	public: 
		iterator(int* _arr) : pos(_arr) {}
		iterator operator++() 
		{
			pos++;
			return *this;
		}
		int operator*() 
		{
			return *pos;
		}
		bool operator!=(iterator other)
		{
			return pos != other.pos;
		}
	};
	const iterator begin()
	{
		return arr;
	}
	const iterator end()
	{
		return arr + 1;
	}
};