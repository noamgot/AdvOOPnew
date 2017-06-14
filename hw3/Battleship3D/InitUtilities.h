#pragma once
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
	 * (in a full path form) and config file in the corresponding vectors/string.	 */
	void filterDirFiles(const std::vector<std::string>& dirFiles, std::vector<std::string>& boardFiles,
	                    std::vector<std::string>& dllFiles, std::string& configFile);

	/* checks if the string "line" ends with the given suffix (return true if it does and false o.w) */
	bool endsWith(const std::string line, const std::string suffix);

	/* loads the game parameters defined in the config file located at cfgPath 
	 * in case of failure, continues with default values */
	int loadConfig(const std::string dirPath, const std::string cfgPath, int& numThreads, std::shared_ptr<Logger> pLogger);

	/* checks a minimal requirements for boards count (boardsCnt >= 1) and players count (playersCnt >= 2)
	 * returns 0 if those requirements are met and -1 otherwise	 
	 * the "filteredLists" flag is for distinguishing between the printed errors	 */
	int checkMinBoardsAndPlayersCount(size_t boardsCnt, size_t playersCnt, const std::string& dirPath, 
										bool filteredLists, std::shared_ptr<Logger> pLogger);

	/* given a vector of paths to sboard files, loads only the valid ones into vector<MyBoardData> boards */
	void initBoards3D(const std::vector<std::string>& boardPaths, std::vector<MyBoardData>& boards, const std::string& dirPath, std::shared_ptr<Logger> pLogger);

	/* parses the dimension line (first line in .sboard file) into rows, columns and depth */
	int getDims(const std::string& line, int& rows, int& cols, int& depth);

	/* checks if the passed board is valid */
	int checkBoard3D(MyBoardData& board, std::shared_ptr<Logger> pLogger);

	/* checks if the ship's shape starting at board[i][j][k] is valid */
	int checkShape3D(MyBoardData& board, const int size, int i, int j, int k);

	/* registers the ship starting at board[i][j][k] to the passed board
	* kDir, iDir, jDir indicates the "direction vector" of the ship
	* i.e if kDir = 0, iDir = 1, jDir = 0, the ship orientation is vertrical
	*/
	void registerShip(MyBoardData& board, const int size, int i, int j, int k, int iDir, int jDir, int kDir);

	/* init all the individual boards for players A and B*/
	void initIndividualBoards(const std::vector<MyBoardData>& boards, std::vector<MyBoardData>& boardA, std::vector<MyBoardData>& boardB);
	
	/* prints the various error encountered on the board (if any)*/
	int logBoardErrors(std::bitset<4>& errShipsA, std::bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck, const MyBoardData& board, std::shared_ptr<Logger> pLogger);

	/* helper functions for printBoardErrors */
	void logWrongSizeOrShapeError(std::bitset<4>& errShips, int& ret, const std::string player, std::shared_ptr<Logger> pLogger);
	void logBadShipsCountError(int shipCountA, int shipCountB, const MyBoardData& board, std::shared_ptr<Logger> pLogger);

}
