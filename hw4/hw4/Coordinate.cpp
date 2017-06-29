#include "Coordinate.h"
Coordinate::Coordinate(int n, std::vector<int> dims) : _position(dims.size()), _dimensions(static_cast<int>(dims.size())), _matloc(n)
{
	for (auto j = _dimensions - 1; j >= 0; j--)
	{
		_position[j] = n % dims[j];
		auto dimOffset = dims[j];
		n = n / dimOffset;
	}
}

Coordinate::Coordinate(int n, int* dims, int size) : _position(size), _dimensions(size), _matloc(n)
{
	for (auto j = _dimensions - 1; j >= 0; j--)
	{
		_position[j] = n % dims[j];
		auto dimOffset = dims[j];
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
		if (_position[i] > other[i])
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
