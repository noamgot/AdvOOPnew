#ifndef SHIP_H
#define SHIP_H

#include <map>
#include "IBattleshipGameAlgo.h"

//#define BOAT 'B'
//#define MISSLE_SHIP 'P'
//#define SUBMARINE 'M'
//#define DESTROYER 'D'
//#define WATER ' '
//TODO @Noam consider moving it to GameUtilities.h ...
enum eShipChar : char
{
	BOAT = 'B',
	MISSLE_SHIP = 'P',
	SUBMARINE = 'M',
	DESTROYER = 'D',
	WATER = ' '
};

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
	Ship(int size, eShipType type, std::map<std::pair<int,int>, bool> coordinates);
	Ship();
	eShipType getType() const;
	std::map<std::pair<int,int>, bool> getCoordinates() const;

	/*Update the ship's after it gets a hit. return true if a real hit occurs
	 * (i.e a "living" ship tile is hit) */
	bool handleHit(std::pair<int,int> coords, AttackResult& res);

	// translate a given character to the corresponding ship type
	static eShipType charToShipType(char c);

	//void setType(eShipType type);
	//void setCoordinates(std::map<std::pair<int,int>, bool> coordinates);
	//void setSize(int size);
	int getSize();
	//bool isAlive();


};


#endif //SHIP_H
