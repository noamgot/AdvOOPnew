#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>

class MyBoardData : public BoardData
{
public:
	MyBoardData() = delete;
	MyBoardData(int rows, int cols, int depth);

	char charAt(Coordinate c) const override { return _board[c.row][c.col][c.depth]; }

private:
	std::vector<std::vector<std::vector<char>>> _board; // 3D board!
	
};
