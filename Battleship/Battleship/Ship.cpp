#include "Ship.h"


bool Ship::handleHit(std::pair<int,int> coords, AttackResult& res)
{
	if (this->mSize <= 0) // ship is already sunk
	{
		res = AttackResult::Miss;
		return false; // to notify the caller that a dead tile was hit
	}

	auto initialTileStatus = this->mCoordinates[coords];
	res = AttackResult::Hit;
	if(initialTileStatus) // if the hit tile was "alive"
	{
		// update ship coordinates and size
		this->mCoordinates[coords] = false;
		this->mSize--;
		if (this->mSize == 0) //  ship is sunk - report "sink" 
		{
			res = AttackResult::Sink; 
		}
	}	
	return initialTileStatus;
}






