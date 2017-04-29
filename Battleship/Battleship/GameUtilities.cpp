
#include "GameUtilities.h"
#include "Graphics.h"
#include <iostream>
#include "InputUtilities.h"

using namespace std;

// initialize static consts

const string GameUtilities::PARAM_QUIET = "-quiet";
const string GameUtilities::PARAM_DELAY = "-delay";

int GameUtilities::processInputArguments(int argc, char** argv, bool& playWithGraphics, int& sleepTime,
										 string& dirPath, string& boardPath, string& atkPathA, string& atkPathB)
{
	
	auto gotDirPath = false;
	if (argc >= 2)
	{ // we accept the arguments in any order, and we assume that if a folder path is given it is the first argument
		for (auto i = 1; i < argc; ++i)
		{
			if (!strcmp(argv[i], PARAM_QUIET.c_str()))
			{
				playWithGraphics = false;
			}
			else if (!strcmp(argv[i], PARAM_DELAY.c_str()))
			{
				if (i + 1 < argc)
				{
					char *p;
					auto delay = strtol(argv[i + 1], &p, 10);
					if (!*p)
					{
						sleepTime = static_cast<DWORD>(delay);
					}
					// if there's no integer after PARAM_DELAY - we ignore this parameter...
				}
			}
			else if (i == 1)
			{ // we assume that if there's a folder path it is the first argument
				dirPath = argv[1];
				if (dirPath.front() == '\\' || dirPath.front() == '/') // if path is relative convert to absolute
				{
					dirPath = getDirPath() + dirPath;
				}
				if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
				{
					return -1;
				}
				boardPath = dirPath + "/" + boardPath;
				atkPathA = dirPath + "/" + atkPathA;
				atkPathB = dirPath + "/" + atkPathB;
				gotDirPath = true;
			}
		}
	}
	if (!gotDirPath) // directory path given
	{
		dirPath = getDirPath();
		if (dirPath == BAD_STRING) //error occurred in getDirPath()
		{
			perror("Error");
			return -1;
		}
		if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
		{
			return -1;
		}
	}

	//success
	return 0;
}

eShipType GameUtilities::charToShipType(char c)
{
	switch (toupper(c))
	{
	case BOAT:
		return eShipType::SHIP_TYPE_B;
	case MISSLE_SHIP:
		return eShipType::SHIP_TYPE_P;
	case SUBMARINE:
		return eShipType::SHIP_TYPE_M;
	case DESTROYER:
		return eShipType::SHIP_TYPE_D;
	default:
		return eShipType::SHIP_TYPE_ERROR;
	}
}

bool GameUtilities::isIllegalMove(std::pair<int, int> move, int numOfRows, int numOfCols)
{
	auto x = move.first;
	auto y = move.second;
	return (x < 1 || x > numOfRows || y < 1 || y > numOfCols);
}

int GameUtilities::calculateSinkScore(char c)
{
	switch (toupper(c))
	{
	case BOAT:
		return BOAT_SCORE;
	case MISSLE_SHIP:
		return MISSLE_SHIP_SCORE;
	case SUBMARINE:
		return SUBMARINE_SCORE;
	case DESTROYER:
		return DESTROYER_SCORE;
	default:
		return -1;
	}
}

void GameUtilities::changeCurrentPlayer(int *attackerNum, int *defenderNum)
{
	*attackerNum = *attackerNum ? 0 : 1;
	*defenderNum = *defenderNum ? 0 : 1;
}


void GameUtilities::printGameResults(AbstractPlayer *pPlayers[], int scores[], bool playWithGraphics)
{	
	if (playWithGraphics)
	{
		Graphics::clearLastLine();
	}
	if (!pPlayers[0]->hasShips())
	{
		cout << "Player B won" << endl;
	}
	else if (!pPlayers[1]->hasShips())
	{
		cout << "Player A won" << endl;
	}
	cout << "Points:\nPlayer A:" << scores[0] << "\nPlayer B:" << scores[1] << endl;
}


char** GameUtilities::allocateBoard(int rows, int cols)
{
	auto board = new char *[rows];
	for (auto i = 0; i < rows; ++i)
	{
		board[i] = new char[cols];
	}
	return board;
}

void GameUtilities::deleteBoard(char** board, int rows)
{
	for (auto i = 0; i < rows; ++i)
	{
		delete[] board[i];
	}
	delete[] board;
}
