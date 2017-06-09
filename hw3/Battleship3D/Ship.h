#pragma once

#include <map>
#include "IBattleshipGameAlgo.h"

namespace std
{
	template<> struct less<Coordinate>
	{
		bool operator() (const Coordinate& lhs, const Coordinate& rhs) const
		{
			return sqrt(pow(double(lhs.row), 2) + pow(double(lhs.col), 2) + pow(double(lhs.depth), 2))
					< sqrt(pow(double(rhs.row), 2) + pow(double(rhs.col), 2) + pow(double(rhs.depth), 2));
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



	Ship::Ship(int size, eShipType type, std::map<Coordinate, bool> coordinates) : _size(size), _coordinates(coordinates),	_type(type)	{}

	Ship() : _size(0), _type()	{}

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