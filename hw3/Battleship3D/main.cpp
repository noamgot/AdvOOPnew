#include "GameUtilities.h"
#include <string>
#include "MyBoardData.h"
#include <iostream>
#include "CompetitionManager.h"


using namespace std;
using namespace CommonUtilities;


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
		if (endsWith(file, GameUtilities::BOARD_FILE_SUFFIX))
		{
			boardFiles.push_back(fullPath);
		}
		else if (endsWith(file, GameUtilities::LIB_FILE_SUFFIX))
		{
			dllFiles.push_back(fullPath);
			playerNames.push_back(removeSuffix(file));
		}

	}
}



int main(int argc, char** argv)
{
	vector<MyBoardData> boards;
	vector<GetAlgoFuncType> algos;
	AlgorithmLoader algoLoader;
	string dirPath;
	auto error = false;
	int numThreads = CompetitionManager::DEFAULT_NUM_THREADS;
	if (GameUtilities::processInputArguments(argc, argv, dirPath, numThreads) < 0)
	{
		return -1;
	}
	if (!isValidPath(dirPath))
	{
		cout << "Wrong Path: " << dirPath << endl;
		return -1;
	}
	// intialize logger only after there is a valid path
	shared_ptr<Logger> pLogger = make_shared<Logger>(dirPath);

	vector<string> dirFiles, boardFiles, dllFiles, playerNames;
	if (getDirectoryFileList(dirPath, dirFiles) < 0)
	{
		char errMsg[BUF_SIZE];
		if(strerror_s(errMsg, BUF_SIZE, errno))
		{
			pLogger->writeToLog("Error: strerro_s failed", true, Logger::eLogType::LOG_ERROR);
		}
		else
		{
			pLogger->writeToLog("Error: " + string(errMsg), true, Logger::eLogType::LOG_ERROR);
		}
		return -1;
	}
	filterDirFiles(dirFiles, boardFiles, dllFiles, playerNames, dirPath);
	if (boardFiles.size() == 0)
	{
		pLogger->writeToLog("No board files (*.sboard) looking in path: " + dirPath, true, Logger::eLogType::LOG_ERROR);
		error = true;
	}
	if (dllFiles.size() < 2)
	{
		pLogger->writeToLog("Missing algorithm (dll) files looking in path: " + dirPath + " (needs at least two)", true, Logger::eLogType::LOG_ERROR);
		error = true;
	}
	if (error)
	{
		return -1;
	}
	GameUtilities::initBoards3D(boardFiles, boards);
	algoLoader.loadLibs(dllFiles);
	algoLoader.exportAlgos(algos);

	//TODO - exit if no valid players/board?
	//vector<MyBoardData> gameBoards(2);
	//vector<GetAlgoFuncType> players(playerNames.size());
	// here we know that we have valid boards and players
	pLogger->writeToLog("Number of legal players: " + to_string(algos.size()), true);
	pLogger->writeToLog("Number of legal boards: " + to_string(boards.size()), true);
	CompetitionManager tournamentMngr(boards, algos, playerNames, pLogger, numThreads);
	tournamentMngr.runCompetition();
	pLogger->writeToLog("", false, Logger::eLogType::LOG_END);
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
