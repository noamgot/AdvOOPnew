#pragma once
#include <string>

struct Game
{
	int boardID;
	int idA;
	int idB;

	// default ctor is a "poisoned" game object
	Game() : boardID(-1), idA(-1), idB(-1) {}
	Game(int boardID_, int idA_, int idB_) : boardID(boardID_), idA(idA_), idB(idB_) {}
};
