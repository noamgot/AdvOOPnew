#pragma once

#include "Ship.h"
#include <vector>

namespace GameUtilities
{

	const int DEFAULT_SHIPS_COUNT = 5;
	const int BUF_SIZE = 1024;

	enum eShipChar : char
	{
		BOAT = 'B',
		MISSLE_SHIP = 'P',
		SUBMARINE = 'M',
		DESTROYER = 'D',
		WATER = ' '
	};

	enum class eFileType
	{
		BOARD_FILE,
		DLL_FILE,
	};

	bool isValidPath(const std::string dirPath);

	/* translate a given character to the corresponding ship type */
	eShipType charToShipType(char c);

	int findFileBySuffix(std::string& filePath, const std::string dirPath, const std::string suffix,
	                     bool& fileNotFound, int playerNum, bool allowSingleFile);

	/*returns true iff the move is legal*/
	inline bool isLegalMove(int x, int y, int z,  int numOfRows, int numOfCols, int depth)
	{
		return !(x < 1 || x > numOfRows || y < 1 || y > numOfCols || z > depth || z < 1);
	}

	inline bool isLegalMove(Coordinate move, int numOfRows, int numOfCols, int depth)
	{
		return isLegalMove(move.row, move.col, move.depth, numOfRows ,numOfCols, depth);
	}

	/* functions for allocating and deleteing boards of size rows*cols */
	char** allocateBoard(int rows, int cols);
	void deleteBoard(char ** board, int rows);

	/* fills the fileListVector with a list of the directory contents.
	 * returns 0 on success and -1 on failure */
	int getDirectoryFileList(const std::string dirPath, std::vector<std::string>& fileListVector);

	/* filter the given file list so its elements which end with the given suffix are put in filteredFileList */
	void filterAndSortFileList(const std::vector<std::string>& fileList, std::vector<std::string>& filteredFileList, const std::string suffix);

	/* checks if the string "line" ends with the given suffix (return true if it does and false o.w) */
	bool endsWith(const std::string line, const std::string suffix);


	/* gets the file path according to dirPath and the given file type.
	 * it also converts the file path to its full path reresentation.
	 * returns a negative number in case of failure, and 0 otherwise */
	int getPathByType(std::string& filePath, const std::string dirPath, const std::string fileSuffix, 
						eFileType fileType, int playerNum);

	// an overload for cases we need only one file of that type (i.e board game)
	inline int getPathByType(std::string& filePath, const std::string dirPath, const std::string fileSuffix)
	{
		return getPathByType(filePath, dirPath, fileSuffix, eFileType::BOARD_FILE, 0);
	}

}