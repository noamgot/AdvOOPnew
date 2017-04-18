#ifndef HW1_SHIP_H
#define HW1_SHIP_H

#include <vector>
#include <map>
#include "IBattleshipGameAlgo.h"

#define BOAT 'B'
#define MISSLE_SHIP 'P'
#define SUBMARINE 'M'
#define DESTROYER 'D'
#define WATER ' '

#define BOAT_SCORE 2
#define MISSLE_SHIP_SCORE 3
#define SUBMARINE_SCORE 7
#define DESTROYER_SCORE 8

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
    int size;
    //  a map of coordinates = if <x,y> is true it means this coordinate was not hit
    std::map<std::pair<int,int>, bool > coordinates;
    eShipType type;

public:
    Ship(int size, eShipType type, std::map<std::pair<int,int>, bool> coordinates);
    Ship();
    eShipType getType();
    std::map<std::pair<int,int>, bool> getCoordinates();

    /*Update the ship's after it gets a hit. return true if a real hit occurs
     * (i.e a "living" ship tile is hit) */
    bool handleHit(std::pair<int,int> coords, AttackResult& res);

    //void setType(eShipType type);
    //void setCoordinates(std::map<std::pair<int,int>, bool> coordinates);
    //void setSize(int size);
    //int getSize();
    //bool isAlive();


};


#endif //HW1_SHIP_H
