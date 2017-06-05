#include "MyBoardData.h"

MyBoardData::MyBoardData(int rows, int cols, int depth)
	: _board(rows, std::vector<std::vector<char>>(cols, std::vector<char>(depth))), _ships(2, std::vector<Ship>())

{
	_rows = rows;
	_cols = cols;
	_depth = depth;
}

MyBoardData::MyBoardData(BoardData &board)
{

}



