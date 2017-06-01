#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
#include <memory>

class MyBoardData : public BoardData
{
public:
	MyBoardData() = default;
	MyBoardData(int rows, int cols, int depth);
	MyBoardData(BoardData &board);
	char charAt(Coordinate c) const override { return _board[c.row][c.col][c.depth]; }
	void setChar(Coordinate coor, char ch) { _board[coor.row][coor.col][coor.depth] = ch; }

private:
	std::vector<std::vector<std::vector<char>>> _board; // 3D board!
	
};
