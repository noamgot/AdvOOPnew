#pragma once
#include <vector>
#include <thread>
#include <iostream>
#include <tuple>
#include <windows.h>
#include <string>
#include <iomanip>
#include <bitset>
#include <mutex>
#include "Ship.h"


namespace CommonUtilities
{
	template <typename T>
	using vector2D = std::vector<std::vector<T>>;
	template <typename T>
	using vector3D = std::vector<std::vector<std::vector<T>>>;
	typedef vector3D<char> RawBoard;

	const int BUF_SIZE = 1024;

	enum eShipChar : char
	{
		BOAT = 'B',
		MISSLE_SHIP = 'P',
		SUBMARINE = 'M',
		DESTROYER = 'D',
		WATER = ' '
	};

	/* translate a given character to the corresponding ship type */
	Ship::eShipType charToShipType(char c);


	/*returns true iff the move is legal*/
	inline bool isLegalMove(int row, int col, int depth,  int numOfRows, int numOfCols, int depthSize)
	{
		return !(row < 1 || row > numOfRows || col < 1 || col > numOfCols || depth > depthSize || depth < 1);
	}

	inline bool isLegalMove(Coordinate move, int numOfRows, int numOfCols, int depthSize)
	{
		return isLegalMove(move.row, move.col, move.depth, numOfRows ,numOfCols, depthSize);
	}
}