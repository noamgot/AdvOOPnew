#pragma once

class Coordinate
{
public:
	int arr[2];

	Coordinate(int y, int x)
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
