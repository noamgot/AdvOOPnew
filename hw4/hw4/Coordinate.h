#pragma once
#include <vector>
#include <list>

class Coordinate
{
private:
	std::vector<int> _position;
	int _dimensions;
	int _matloc = 0;

public:
	
	Coordinate(int row, int col) : _position{ row, col}, _dimensions(2) {}

	Coordinate(int row, int col, int depth) : _position{ row, col, depth }, _dimensions(3)  {}

	Coordinate(std::initializer_list<int> v) : _position(v), _dimensions(v.size()) {}

	Coordinate(std::vector<int> v) : _position(v), _dimensions(v.size()) {}

	Coordinate(int n, std::vector<int> dims) : _matloc(n), _dimensions(dims.size()), _position(dims.size())
	{
		int dimOffset = 1;
		for (int j = _dimensions - 1; j >= 0; j--)
		{
			_position[j] = n % dims[j];
			dimOffset = dims[j];
			n = n / dimOffset;
		}
	}

	Coordinate changeDim(int dim, int offset, int matloc)
	{
		std::vector<int> _new(_position);
		_new[dim] += offset;
		Coordinate coor = Coordinate(_new);
		coor._matloc = matloc;
		return coor;
	}

	int getRow() const { return _position[0]; }

	int getCol() const { return _position[1]; }

	int getDepth() const { return _position[2]; }

	int getMatloc() const { return _matloc; }

	auto begin() const { return _position.begin(); }

	auto end() const { return _position.end(); }

	bool operator< (const Coordinate& other)
	{
		/*if (getRow() < other.getRow()) { return true; }
		else if (getRow() == other.getRow())
		{
			if (getCol() < other.getCol()) { return true; }
			else if (getCol() == other.getCol())
			{
				if (getDepth() < other.getDepth()) { return true; }
			}
		}*/
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

};
