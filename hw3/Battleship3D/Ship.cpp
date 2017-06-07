#include "Ship.h"


bool Ship::handleHit(Coordinate coords, AttackResult& res)
{
	if (this->_size <= 0) // ship is already sunk
	{
		res = AttackResult::Miss;
		return false; // to notify the caller that a dead tile was hit
	}

	auto initialTileStatus = this->_coordinates[coords];
	res = AttackResult::Hit;
	if(initialTileStatus) // if the hit tile was "alive"
	{
		// update ship coordinates and size
		this->_coordinates[coords] = false;
		this->_size--;
		if (this->_size == 0) //  ship is sunk - report "sink" 
		{
			res = AttackResult::Sink; 
		}
	}	
	return initialTileStatus;
}