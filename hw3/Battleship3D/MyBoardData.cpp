#include "MyBoardData.h"

MyBoardData::MyBoardData(int rows, int cols, int depth, int playerNum, std::vector<std::vector<std::vector<char>>> * pBoard)
//: _board(rows, std::vector<std::vector<char>>(cols, std::vector<char>(depth) ) )
	: _playerNum(playerNum), _pBoard(pBoard)
{
	_rows = rows;
	_cols = cols;
	_depth = depth;
}

