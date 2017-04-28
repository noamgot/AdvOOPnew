#pragma once

#include "Ship.h"
#include "AbstractPlayer.h"

using namespace std;


class GameUtilities
{
	
public:
	static const string PARAM_QUIET;
	static const string PARAM_DELAY;

	static int processInputArguments(int argc, char** argv, bool& playWithGraphics, int& sleepTime,
	                                 string& boardPath, string& atkPathA, string& atkPathB);

	static eShipType charToShipType(char c);

	static int calculateSinkScore(char c);

	static void changeCurrentPlayer(int *attackerNum, int *defenderNum);

	static void printGameResults(AbstractPlayer *pPlayers[], int scores[], bool playWithGraphics);

	static char** allocateBoard(int rows, int cols);

	static void deleteBoard(char ** board, int rows);

};



