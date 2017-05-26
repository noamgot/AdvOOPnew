#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>

class MyBoardData : public BoardData
{
	std::vector<std::vector<std::vector<char>>> _board; // 3D board!
public:
	MyBoardData() = delete;
	MyBoardData(int rows, int cols, int depth);
};
