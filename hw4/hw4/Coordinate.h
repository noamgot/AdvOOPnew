#pragma once
#include <vector>

class Coordinate
{
private:
	std::vector<int> _position;
public:
	
	Coordinate(int row, int col) : _position{ row, col} {}

	Coordinate(int row, int col, int depth) : _position{ row, col, depth }  {}

	int getRow() const { return _position[0]; }

	int getCol() const { return _position[1]; }

	int getDepth() const { return _position[2]; }

	auto begin() const { return _position.begin(); }

	auto end() const { return _position.end(); }

	bool operator< (const Coordinate& other)
	{
		if (getRow() < other.getRow()) { return true; }
		else if (getRow() == other.getRow())
		{
			if (getCol() < other.getCol()) { return true; }
			else if (getCol() == other.getCol())
			{
				if (getDepth() < other.getDepth()) { return true; }
			}
		}
		return false;
	}
};
