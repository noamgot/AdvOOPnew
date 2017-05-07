#pragma once

#include "Ship.h"
#include <windows.h>
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

class GameUtilities
{
	
public:
	
	static const int DEFAULT_SHIPS_COUNT;
	static const int BUF_SIZE;

	static bool isValidPath(const string dirPath);

	/* translate a given character to the corresponding ship type */
	static eShipType charToShipType(char c);	

	static int findFileBySuffix(string& filePath, const string dirPath, const string suffix,
								bool& fileNotFound, int playerNum);

	/*return true iff the move is legal*/
	static bool isLegalMove(int x, int y, int numOfRows, int numOfCols)
	{
		return !(x < 1 || x > numOfRows || y < 1 || y > numOfCols);
	}


	static char** allocateBoard(int rows, int cols);

	static void deleteBoard(char ** board, int rows);

private:

	static int getDirectorySortedFileList(const string dirPath, vector<string>& fileListVector);

	static bool endsWith(const string line, const string suffix);

};