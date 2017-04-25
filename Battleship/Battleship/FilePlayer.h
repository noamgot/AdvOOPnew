#pragma once
#include "DeterministPlayer.h"
class FilePlayer :	public DeterministPlayer
{
public:
	FilePlayer();
	~FilePlayer();
	void setMoves(vector<pair<int, int>> moves);
};

