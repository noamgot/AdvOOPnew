#pragma once
#include <vector>
#include <list>

class Coordinate
{
	std::vector<int> _position;
	int _dimensions;
	int _matloc = 0;
public:
	Coordinate(std::vector<int> v) : _position(v), _dimensions(static_cast<int>(v.size())) {}

	Coordinate(int n, std::vector<int> dims);

	Coordinate(int n, int* dims, int size);

	int getMatloc() const { return _matloc; }

	auto begin() const { return _position.begin(); }

	auto end() const { return _position.end(); }

	bool operator< (const Coordinate& other)
	{
		for (int i = 0; i < _dimensions; i++)
		{
			if (_position[i] < other[i])
			{
				return true;
			}
			else if ( _position[i] > other[i] )
			{
				return false;
			}
		}
		return false;
	}

	int operator[] (int dim) const { return (dim < _dimensions && dim >= 0) ? _position[dim] : 0; }

	Coordinate changeDim (int dim, int offset, int matloc) const
	{
		std::vector<int> _new(_position);
		_new[dim] += offset;
		Coordinate coor = Coordinate(_new);
		coor._matloc = matloc;
		return coor;
	}
};
