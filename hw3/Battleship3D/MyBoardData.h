#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
#include <memory>

class MyBoardData : public BoardData
{
public:
	MyBoardData(int rows, int cols, int depth, int playerNum, std::vector<std::vector<std::vector<char>>> * pBoard);

	char charAt(Coordinate c) const override { return _board[c.row][c.col][c.depth]; }
	void setChar(Coordinate coor, char ch) { _board[coor.row][coor.col][coor.depth] = ch; }

private:
	int _playerNum;
	std::unique_ptr<std::vector<std::vector<std::vector<char>>>> _pBoard;
	std::vector<std::vector<std::vector<char>>> _board; // 3D board!
	
};
