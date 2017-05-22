#pragma once

#include "Coordinate.h"

class BoardData
{
public:
	int rows() const;
	int cols() const;
	int depth() const;
	virtual ~BoardData() { }
	virtual char charAt(Coordinate c) const = 0;
};
