#pragma once
#include <ctime>
#include <bitset>
#include "GameUtilities.h"
#include "DLLManager.h"

using namespace std;

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


	int processInputArguments(int argc, char** argv, string& dirPath);

	void changeCurrentPlayer(int& attackerNum, int& defenderNum, string& attackerName);

	void printGameResults(PlayerAttributes playerAttributesArr[]);

	/* Initializes the battle boards according to the .sboard file in boardPath.
	* results in a rows*cols board inside passed boards arguments - including
	* individual boards for both players									  */
	int initGameBoards(const string boardPath, string board[], char** boardA, char** boardB);

	/* gets the board file path according to dirPath. it also changes dirrPath to be a full path repr. of the path.
	 * returns a negative number in case of failure, and 0 otherwise*/
	int getBoardPath(string& dirPath, string& boardPath);

	int getDllPath(string& dirPath, string& dllPath, int playerNum);

	bool initPlayer(IBattleshipGameAlgo*& pPlayer, int playerNum, const char** board, const string dirPath, const string dllPath, 
					PlayerAttributes playerAttributesArr[], DLLManager& dllMngr);

	bool registerHit(PlayerAttributes& playerAttributes, pair<int, int> coords, eShipType shipType, AttackResult& res);

	int calculateSinkScore(char c);

	int playTheGame(IBattleshipGameAlgo* A, IBattleshipGameAlgo* B, PlayerAttributes playerAttributesArr[], const string* board);

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