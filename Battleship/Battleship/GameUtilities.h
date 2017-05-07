#pragma once

#include "Ship.h"
#include <vector>

using namespace std;

enum eShipChar : char
{
	BOAT = 'B',
	MISSLE_SHIP = 'P',
	SUBMARINE = 'M',
	DESTROYER = 'D',
	WATER = ' '
};

namespace GameUtilities
{
	const int DEFAULT_SHIPS_COUNT = 5;
	const int BUF_SIZE = 1024;

	bool isValidPath(const string dirPath);

	/* translate a given character to the corresponding ship type */
	eShipType charToShipType(char c);

	int findFileBySuffix(string& filePath, const string dirPath, const string suffix,
		bool& fileNotFound, int playerNum);

	/*return true iff the move is legal*/
	inline bool isLegalMove(int x, int y, int numOfRows, int numOfCols)
	{
		return !(x < 1 || x > numOfRows || y < 1 || y > numOfCols);
	}

	char** allocateBoard(int rows, int cols);

	void deleteBoard(char ** board, int rows);

	int getDirectorySortedFileList(const string dirPath, vector<string>& fileListVector);

	bool endsWith(const string line, const string suffix);

}