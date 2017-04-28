#pragma once
#include "Ship.h"

enum class ePlayerIdentifier
{
	PLAYER_A,
	PLAYER_B
};

class MarkedShip : public Ship
{
	ePlayerIdentifier playerIdentifier;
	
public:
	MarkedShip();
	~MarkedShip();
};

