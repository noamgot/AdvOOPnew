#pragma once
#include "Ship.h"
#include "CommonUtilities.h"

class MyBoardData : public BoardData
{
public:
	MyBoardData() = default;
	MyBoardData::MyBoardData(int rows, int cols, int depth) 
	: _board(rows, std::vector<std::vector<char>>(cols, std::vector<char>(depth))), _ships(2, std::vector<Ship>())
	{
		_rows = rows;
		_cols = cols;
		_depth = depth;
	}

	//MyBoardData(CommonUtilities::RawBoard& board);

	// if a player gives an invalid char - we return the WATER char (and that's his problem...
	char charAt(Coordinate c) const override 
	{
		return  CommonUtilities::isLegalMove(c, _rows, _cols, _depth) ?
			_board[c.row - 1][c.col - 1][c.depth - 1] : CommonUtilities::WATER; 
	}
	void setChar(Coordinate coor, char ch)
	{
		_board[coor.row - 1][coor.col - 1][coor.depth - 1] = ch;
	}

	CommonUtilities::vector2D<char>& operator[] (int i) { return _board[i]; }

	void copyBoard(CommonUtilities::vector3D<char> board) { _board = board; }
	void addShip(int player_id, const Ship& ship) { _ships[player_id].push_back(ship); }
	std::vector<Ship> getShipList(const int player_id) const { return _ships[player_id]; }
	size_t getShipCount() const { return _ships.size(); }
private:
	CommonUtilities::RawBoard _board; // 3D board!
	CommonUtilities::vector2D<Ship> _ships;
};
