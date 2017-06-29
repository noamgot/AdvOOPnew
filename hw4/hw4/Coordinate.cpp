#include "Coordinate.h"
Coordinate::Coordinate(int n, std::vector<int> dims) : _matloc(n), _dimensions(static_cast<int>(dims.size())), _position(dims.size())
{
	int dimOffset = 1;
	for (auto j = _dimensions - 1; j >= 0; j--)
	{
		_position[j] = n % dims[j];
		dimOffset = dims[j];
		n = n / dimOffset;
	}
}

Coordinate::Coordinate(int n, int* dims, int size) : _position(size), _dimensions(size), _matloc(n)
{
	int dimOffset = 1;
	for (auto j = _dimensions - 1; j >= 0; j--)
	{
		_position[j] = n % dims[j];
		dimOffset = dims[j];
		n = n / dimOffset;
	}
}

bool Coordinate::operator<(const Coordinate & other)
{
	for (int i = 0; i < _dimensions; i++)
	{
		if (_position[i] < other[i])
		{
			return true;
		}
		else if (_position[i] > other[i])
		{
			return false;
		}
	}
	return false;
}

Coordinate Coordinate::changeDim(int dim, int offset, int matloc) const
{
	std::vector<int> _new(_position);
	_new[dim] += offset;
	Coordinate coor = Coordinate(_new);
	coor._matloc = matloc;
	return coor;
}
