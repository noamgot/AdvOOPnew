#pragma once

#include <map>
#include "IBattleshipGameAlgo.h"


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

class Ship
{
	int mSize;
	//  a map of coordinates = if <x,y> is true it means this coordinate was not hit
	std::map<std::pair<int,int>, bool > mCoordinates;
	eShipType mType;

public:
	Ship::Ship(int size, eShipType type, std::map<std::pair<int, int>, bool> coordinates) :
		mSize(size),
		mCoordinates(coordinates),
		mType(type)
	{}

	Ship() : mSize(0), mType()	{}

	/* getter for the ship type */
	eShipType getType() const { return mType; }

	/* getter for the ship's coordinates */
	std::map<std::pair<int, int>, bool> getCoordinates() const { return  mCoordinates; }

	int getSize() const { return mSize; }

	/*Update the ship's after it gets a hit. return true if a real hit occurs
	 * (i.e a "living" ship tile is hit) */
	bool handleHit(std::pair<int,int> coords, AttackResult& res);	

	
};