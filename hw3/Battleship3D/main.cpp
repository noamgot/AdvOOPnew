#include "GameManagerUtilities.h"
#include <ctime>
#include "MyBoardData.h"
#include <iostream>

using namespace std;
using namespace GameManagerUtilities;
using namespace GameUtilities;


void filterDirFiles(const vector<string>& dirFiles, vector<string>& boardFiles, vector<string>& dllFiles, const string dirPath)
{
	// copy all relevant files to the filteredFileList vector
	for (auto& file : dirFiles)
	{
		auto fullPath = dirPath + "\\" + file;
		if (endsWith(file, BOARD_FILE_SUFFIX))
		{
			boardFiles.push_back(fullPath);
		}
		else if (endsWith(file, LIB_FILE_SUFFIX))
		{
			dllFiles.push_back(fullPath);
		}

	}
}

int main(int argc, char** argv)
{

	string dirPath;
	int numThreads;
	if (GameManagerUtilities::processInputArguments(argc, argv, dirPath, numThreads) < 0)
	{
		return -1;
	}
	if (!isValidPath(dirPath))
	{
		cout << "Wrong Path: " << dirPath << endl;
		return -1;
	}

	vector<string> dirFiles, boardFiles, dllFiles;
	if (getDirectoryFileList(dirPath, dirFiles) < 0)
	{
		return -1;
	}
	filterDirFiles(dirFiles, boardFiles, dllFiles, dirPath);






	/*string dirPath, board[ROW_SIZE], dllPathA, dllPathB;
	DLLManager dllMngr;
	PlayerAttributes playerAttributesArr[2];
	IBattleshipGameAlgo *A, *B;

	// allocate individual boards
	auto boardA = allocateBoard(ROW_SIZE, COL_SIZE), boardB = allocateBoard(ROW_SIZE, COL_SIZE);

	// initialize game, boards and paths
	if (initGameBoardsAndPaths(argc ,argv, board, boardA, boardB, dirPath, dllPathA, dllPathB) < 0)
	{
		return EXIT_FAILURE;
	}
	
	// initialize players and player attributes
	if (!initPlayer(A, 0, const_cast<const char **>(boardA), dirPath, dllPathA, playerAttributesArr, dllMngr) ||
		!initPlayer(B, 1, const_cast<const char **>(boardB), dirPath, dllPathB, playerAttributesArr, dllMngr))
	{
		return EXIT_FAILURE;
	} 

	// delete individual boards - we do not need them anymore
	deleteBoard(boardA, ROW_SIZE);
	deleteBoard(boardB, ROW_SIZE);

	// Let the game begin!!!
	return playTheGame(A, B, playerAttributesArr, board);*/
}