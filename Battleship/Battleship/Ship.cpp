#include "Ship.h"

Ship::Ship(int size, eShipType type, std::map<std::pair<int,int>, bool> coordinates)
{
    this->size = size;
    this->type = type;
    this->coordinates = coordinates;
}


//void Ship::setType(eShipType type)
//{
//    this->type = type;
//}
//
//void Ship::setCoordinates(std::map<std::pair<int,int>, bool> coordinates)
//{
//    this->coordinates = coordinates;
//}
//
//void Ship::setSize(int size)
//{
//    this->size = size;
//}

eShipType Ship::getType()
{
    return this->type;
}

std::map<std::pair<int,int>, bool> Ship::getCoordinates()
{
    return this->coordinates;
}

//int Ship::getSize()
//{
//    return this->size;
//}

bool Ship::handleHit(std::pair<int,int> coords, AttackResult& res)
{
    // update only until size == 0 (don't allow negative size)
    bool initialTileStatus = this->coordinates[coords];
    if (this->size > 0)
    {
        res = AttackResult::Hit;
        if(initialTileStatus) // if the hit tile was "alive"
        {
            // update ship coordinates and size and report "sink" if the ship is sunk
            this->coordinates[coords] = false;
            this->size--;
            if (this->size == 0)
            {
                res = AttackResult::Sink;
            }
        }
    }
    else // ship is already sunk
    {
        res = AttackResult::Miss;
    }
    return initialTileStatus;
}

//bool Ship::isAlive()
//{
//    return (this->size > 0);
//
//}

Ship::Ship()
{

}

