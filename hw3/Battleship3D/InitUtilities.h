﻿#pragma once
#include <vector>
#include <memory>
#include "Logger.h"
#include "MyBoardData.h"

namespace InitUtilities
{
	/* processing input arguments */
	int processInputArguments(int argc, char** argv, std::string& dirPath, int& numThreads);

	/* returns an absolute path to the current working directory */
	std::string getDirPath();

	/* changes the given dirPath (relative or absolute) to its full path representation */
	int convertToFullPath(std::string& dirPath);

	bool isValidPath(const std::string dirPath);

	/* fills the fileListVector with a list of the directory contents.
	* returns 0 on success and -1 on failure */
	int getDirectoryFileList(const std::string dirPath, std::vector<std::string>& fileListVector, std::shared_ptr<Logger> pLogger);

	/* filters the dirFiles vector (which contatins a list of the directory files) and stores the board files, dllFiles
	 * (in a full path form) and player names (derived from the dll file names) in the corresponding vectors.	 */
	void filterDirFiles(const std::vector<std::string>& dirFiles, std::vector<std::string>& boardFiles,
	                    std::vector<std::string>& dllFiles, const std::string& dirPath);

	/* checks if the string "line" ends with the given suffix (return true if it does and false o.w) */
	bool endsWith(const std::string line, const std::string suffix);

	/* checks a minimal requirements for boards count (boardsCnt >= 1) and players count (playersCnt >= 2)
	 * returns 0 if those requirements are met and -1 otherwise	 
	 * the "filteredLists" flag is for distinguishing between the printed errors	 */
	int checkMinBoardsAndPlayersCount(size_t boardsCnt, size_t playersCnt, const std::string& dirPath, bool filteredLists, std::shared_ptr<Logger> pLogger);

	/* given a vector of paths to sboard files, loads only the valid ones into vector<MyBoardData> boards */
	void initBoards3D(const std::vector<std::string>& boardPaths, std::vector<MyBoardData>& boards);

	/* parses the dimension line (first line in .sboard file) into rows, columns and depth */
	int getDims(const std::string& line, int& rows, int& cols, int& depth);

	/* checks if the passed board is valid */
	int checkBoard3D(MyBoardData& board);

	/* checks if the ship's shape starting at board[i][j][k] is valid */
	int checkShape3D(MyBoardData& board, const int size, int i, int j, int k);

	/* registers the ship starting at board[i][j][k] to the passed board
	* kDir, iDir, jDir indicates the "direction vector" of the ship
	* i.e if kDir = 0, iDir = 1, jDir = 0, the ship orientation is vertrical
	*/
	void registerShip(MyBoardData& board, const int size, int i, int j, int k, int iDir, int jDir, int kDir);

	

}