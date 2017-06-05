#pragma once
#include <vector>
#include "Utilities.h"

class MyBoardData : public BoardData
{
public:
	MyBoardData() = default;
	MyBoardData(int rows, int cols, int depth);
	MyBoardData(BoardData &board);
	char charAt(Coordinate c) const override { return _board[c.row][c.col][c.depth]; }
	void setChar(Coordinate coor, char ch) { _board[coor.row][coor.col][coor.depth] = ch; }
	void addShip(const int& player_id, const Ship& ship) { _ships[player_id].push_back(ship); }
	std::vector<Ship> getShipList(const int& player_id) { return _ships[player_id]; }
private:
	vector3D<char>_board; // 3D board!
	std::vector<std::vector<Ship>> _ships;
	
};
