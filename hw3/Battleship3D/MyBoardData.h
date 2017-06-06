#pragma once
#include "Ship.h"
#include "CommonUtilities.h"

class MyBoardData : public BoardData
{
public:
	MyBoardData() = default;
	MyBoardData(int rows, int cols, int depth);
	MyBoardData(BoardData &board);
	//MyBoardData(CommonUtilities::RawBoard& board);
	char charAt(Coordinate c) const override { return _board[c.row][c.col][c.depth]; }
	std::vector<std::vector<char>>& operator[] (int i) { return _board[i]; }
	void setChar(Coordinate coor, char ch) { _board[coor.row][coor.col][coor.depth] = ch; }
	void setBoard(CommonUtilities::vector3D<char> board) { _board = board; }
	void addShip(int player_id, const Ship& ship) { _ships[player_id].push_back(ship); }
	std::vector<Ship> getShipList(const int player_id) const { return _ships[player_id]; }
	size_t getShipCount() const { return _ships.size(); }
private:
	CommonUtilities::RawBoard _board; // 3D board!
	CommonUtilities::vector2D<Ship> _ships;
};
