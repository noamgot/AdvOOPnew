#pragma once

#include <map>
#include "IBattleshipGameAlgo.h"


// overloading std::less for Coordinate - we need to define a well-defined operator <
namespace std
{
	template<> struct less<Coordinate>
	{
		bool operator() (const Coordinate& lhs, const Coordinate& rhs) const
		{
			if (lhs.row != rhs.row)
			{
				return lhs.row < rhs.row;
			}
			if (lhs.col != rhs.col)
			{
				return lhs.col < rhs.col;
			}
			return lhs.depth < rhs.depth;
		}
	};
}

class Ship
{
public:

	enum eShipScore
	{
		BOAT_SCORE = 2,
		MISSLE_SHIP_SCORE = 3,
		SUBMARINE_SCORE = 7,
		DESTROYER_SCORE = 8,
	};
	enum class eShipType
	{
		SHIP_TYPE_B,
		SHIP_TYPE_P,
		SHIP_TYPE_M,
		SHIP_TYPE_D,
		SHIP_TYPE_ERROR
	};



	Ship::Ship(int size, eShipType type, std::map<Coordinate, bool> coordinates) : _size(size), _coordinates(coordinates), _type(type) {}

	Ship() : _size(0), _type() {}

	/* getter for the ship type */
	eShipType getType() const { return _type; }

	/* getter for the ship's coordinates */
	std::map<Coordinate, bool> getCoordinates() const { return  _coordinates; }

	int getSize() const { return _size; }

	/*Update the ship's after it gets a hit. return true if a real hit occurs
	* (i.e a "living" ship tile is hit) */
	bool handleHit(Coordinate coords, AttackResult& res);

	bool operator < (const Ship& other) const
	{
		return (this->_size < other._size);
	}

private:
	int _size;
	//  a map of coordinates = if _corrdinate[coor] is true it means this coordinate was not hit
	std::map<Coordinate, bool> _coordinates;
	eShipType _type;
};