#include "GameUtilities.h"
#include <ctime>
#include "MyBoardData.h"
#include <iostream>
#include "AlgorithmLoader.h"
#include "CompetitionManager.h"

using namespace std;
using namespace GameUtilities;
using namespace Utilities;


string removeSuffix(const string& filename) 
{
	auto lastdot = filename.find_last_of(".");
	if (lastdot == string::npos)
	{
		return filename;
	}
	return filename.substr(0, lastdot);
}

void filterDirFiles(const vector<string>& dirFiles, vector<string>& boardFiles, 
					vector<string>& dllFiles, vector<string>& playerNames, const string dirPath)
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
			playerNames.push_back(removeSuffix(file));
		}

	}
}

int main(int argc, char** argv)
{
	vector<vector3d> boards;
	AlgorithmLoader dllMngr;
	string dirPath;
	int numThreads = CompetitionManager::DEFAULT_NUM_THREADS;
	if (processInputArguments(argc, argv, dirPath, numThreads) < 0)
	{
		return -1;
	}
	if (!isValidPath(dirPath))
	{
		cout << "Wrong Path: " << dirPath << endl;
		return -1;
	}

	vector<string> dirFiles, boardFiles, dllFiles, playerNames;
	if (getDirectoryFileList(dirPath, dirFiles) < 0)
	{
		return -1;
	}
	filterDirFiles(dirFiles, boardFiles, dllFiles, playerNames, dirPath);
	/*if (boardFiles.size() == 0)
	{
		cout << "No board files (*.sboard) looking in path: " << dirPath << endl;
		return -1;
	}
	if (dllFiles.size() < 2)
	{
		cout << "Missing algorithm (dll) files looking in path: " << dirPath << " (needs at least two)"  << endl;
		return -1;
	}*/
	initBoards3D(boardFiles, boards);
	dllMngr.loadLibs(dllFiles);


	vector<vector3D<char>> gameBoards(2);
	vector<GetAlgoFuncType> players(playerNames.size());
	//playerNames = { "Jordy-Jordalish", "Ben-El Tavorush", "Stat-Boy (Oy-oy-oy)" };
	CompetitionManager tournamentMngr(gameBoards, players, playerNames, numThreads);
	tournamentMngr.runTournament();
	return EXIT_SUCCESS;
}
	
	





	/*string dirPath, board[ROW_SIZE], dllPathA, dllPathB;
	AlgorithmLoader dllMngr;
	PlayerAttributes _playerAttributes[2];
	IBattleshipGameAlgo *A, *B;

	// allocate individual boards
	auto boardA = allocateBoard(ROW_SIZE, COL_SIZE), boardB = allocateBoard(ROW_SIZE, COL_SIZE);

	// initialize game, boards and paths
	if (initGameBoardsAndPaths(argc ,argv, board, boardA, boardB, dirPath, dllPathA, dllPathB) < 0)
	{
		return EXIT_FAILURE;
	}
	
	// initialize players and player attributes
	if (!initPlayer(A, 0, const_cast<const char **>(boardA), dirPath, dllPathA, _playerAttributes, dllMngr) ||
		!initPlayer(B, 1, const_cast<const char **>(boardB), dirPath, dllPathB, _playerAttributes, dllMngr))
	{
		return EXIT_FAILURE;
	} 

	// delete individual boards - we do not need them anymore
	deleteBoard(boardA, ROW_SIZE);
	deleteBoard(boardB, ROW_SIZE);

	// Let the game begin!!!
	return playTheGame(A, B, _playerAttributes, board);*/
