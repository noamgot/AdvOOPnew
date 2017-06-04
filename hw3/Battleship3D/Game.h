#pragma once
#include <string>
#include "Ship.h"
#include <vector>

struct Game
{
	int _boardID;
	int _idA;
	int _idB;

	// default ctor is a "poisoned" game object
	Game() : _boardID(-1), _idA(-1), _idB(-1) {}
	Game(int boardID_, int idA_, int idB_) : _boardID(boardID_), _idA(idA_), _idB(idB_) {}

};
