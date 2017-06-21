#pragma once
#include <initializer_list>

class Coord2d
{
public:
	int arr[2];

	Coord2d(int y, int x)
	{
		arr[0] = y;
		arr[1] = x;
	}

	const int* begin() const {
		return arr;
	}
	const int* end() const {
		return arr + 2;
	}
};