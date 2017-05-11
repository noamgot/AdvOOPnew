#pragma once
#include <ctime>
#include <bitset>
#include "GameUtilities.h"
#include "DLLManager.h"

using namespace std;
using namespace GameUtilities;

#define ROW_SIZE 10
#define COL_SIZE 10


namespace GameManagerUtilities
{
	/* input argument for playing without graphics */
	const string PARAM_QUIET("-quiet");
	/* input argument for setting the delay between operations when playing with graphics */
	const string PARAM_DELAY("-delay");
	const string BOARD_FILE_SUFFIX(".sboard");
	const string LIB_FILE_SUFFIX(".dll");
	const string BAD_STRING("!@#"); // for getDirPath validation
	const int MAX_PATH_LEN = 1024;

	typedef struct _PlayerAttributes
	{
		bool hasMoves;
		int score;
		int shipsCount;
		vector<Ship> shipList;
	}PlayerAttributes;

	int initGameBoardsAndPaths(int argc, char **argv, string(&board)[ROW_SIZE], char **boardA, char* *boardB,
		string& dirPath, string& dllPathA, string& dllPathB);

	int processInputArguments(int argc, char** argv, string& dirPath);

	void changeCurrentPlayer(int& attackerNum, int& defenderNum, string& attackerName);

	void printGameResults(PlayerAttributes playerAttributesArr[]);

	/* Initializes the battle boards according to the .sboard file in boardPath.
	* results in a rows*cols board inside passed boards arguments - including
	* individual boards for both players									  */
	int initGameBoards(const string boardPath, string board[], char** boardA, char** boardB);

	bool initPlayer(IBattleshipGameAlgo*& pPlayer, int playerNum, const char** board, const string dirPath, const string dllPath, 
					PlayerAttributes playerAttributesArr[], DLLManager& dllMngr);

	bool registerHit(PlayerAttributes& playerAttributes, pair<int, int> coords, eShipType shipType, AttackResult& res);

	int calculateSinkScore(char c);

	int playTheGame(IBattleshipGameAlgo* A, IBattleshipGameAlgo* B, PlayerAttributes playerAttributesArr[], const string* board);

	void handleMove(const string *board, pair<int, int> move, int &attackerNum, int &defenderNum, string &attackerName,
		IBattleshipGameAlgo *A, IBattleshipGameAlgo *B, PlayerAttributes playerAttributesArr[]);

	void handleMiss(pair<int, int> move, IBattleshipGameAlgo *A, IBattleshipGameAlgo *B, int attackerNum);	

	void handleHitOrSink(pair<int, int> move, bool &validAttack, IBattleshipGameAlgo *A, IBattleshipGameAlgo *B,
	                     char hitChar, int attackerNum, PlayerAttributes playerAttributesArr[]);

	/* sets 2 individual boards - boardA and boardB - from board*/
	void initIndividualBoards(string *board, char **boardA, char **boardB);

	/* Checks if the ship's shape starting at board[i][j] is valid */
	int checkShape(string* board, const int size, int i, int j);

	/* prints the various error encountered on the board (if any)*/
	int printBoardErrors(bitset<4>& errShipsA, bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck);

	/* helper functions for printBoardErrors */
	void printWrongSizeOrShapeError(bitset<4>& errShips, int& ret, const string player);
	void printBadShipsCountErrror(int shipCount, int& ret, const string player);

	/* validates the given board*/
	int checkBoardValidity(string* board);

	/* changes the given dirPath (relative or absolute) to its full path representation */
	int convertToFullPath(string& dirPath);

	/* returns an absolute path to the current working directory */
	string getDirPath();

	/* initializes the given PlayerAttribute struct, according to the given board*/
	void initPlayersAttributes(PlayerAttributes& playerAttributes, const char** playerBoard);

	/* helper function - initializes the ship list in the given PlayerAttribute struct, according to the given board*/
	void initPlayerShipsList(const char** board, int numOfRows, int numOfCols, PlayerAttributes& playerAttributes);

	/* a helper function to the initPlayerShipsList method */
	Ship handleShipDiscovery(int iOrig, int jOrig, int numOfRows, int numOfCols, const char** board);

}