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

	int operator[] (int dim) const { return (dim < _dimensions && dim >= 0) ? _position[dim] : 0; }

	bool operator< (const Coordinate& other);

	Coordinate changeDim(int dim, int offset, int matloc) const;
};
