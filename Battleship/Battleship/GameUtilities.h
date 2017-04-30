#pragma once

#include "Ship.h"
#include "AbstractPlayer.h"
#include <bitset>

using namespace std;


class GameUtilities
{
	
public:
	static const string PARAM_QUIET;
	static const string PARAM_DELAY;
	static const string BOARD_FILE_SUFFIX;

	static int processInputArguments(int argc, char** argv, bool& playWithGraphics, int& sleepTime,
	                                 string& dirPath, string& boardPath, string& atkPathA, string& atkPathB);

	static eShipType charToShipType(char c);

	/*return true iff the move is ILlegal*/
	static bool isIllegalMove(std::pair<int, int> move, int rows, int cols);

	static int calculateSinkScore(char c);

	static void changeCurrentPlayer(int *attackerNum, int *defenderNum);

	static void printGameResults(AbstractPlayer *pPlayers[], int scores[], bool playWithGraphics);

	static char** allocateBoard(int rows, int cols);

	static void deleteBoard(char ** board, int rows);

	static int initGameBoards(const string boardPath, string* board, char** boardA, char** boardB);

	static void initIndividualBoards(string *board, char **boardA, char **boardB);

	static int checkShape(string* board, const int size, int i, int j);

	static int printBoardErrors(bitset<4>& errShipsA, bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck);

	static int checkBoardValidity(string* board);

	static int findFileBySuffix(string& filePath, const string dirPath, const string suffix);

};



