#pragma once


#include <vector>
#include <bitset>
#include <map>
#include "GameUtilities.h"

using namespace std;

#define ROW_SIZE 10
#define COL_SIZE 10


typedef struct _PlayerAttributes
{
	bool hasMoves;
	int score;
	int shipsCount;
	vector<Ship> shipList;
}PlayerAttributes;

class GameManagerUtilities
{
public:
	static void processInputArguments(int argc, char** argv, bool& playWithGraphics, int& sleepTime, std::string& dirPath);

	static void changeCurrentPlayer(int& attackerNum, int& defenderNum);

	static void printGameResults(PlayerAttributes playerAttributesArr[], bool playWithGraphics);

	/* Initializes the battle boards according to the .sboard file in boardPath.
	* results in a rows*cols board inside passed boards arguments - including
	* individual boards for both players									  */
	static int initGameBoards(const string boardPath, string board[], char** boardA, char** boardB);

	/* gets the board file path according to dirPath. it also changes dirrPath to be a full path repr. of the path.
	 * returns a negative number in case of failure, and 0 otherwise*/
	static int getBoardPath(string& dirPath, string& boardPath);

	static bool initPlayer(IBattleshipGameAlgo* pPlayer, int playerNum, const char** board, const string dirPath, PlayerAttributes playerAttributes[]);

	static bool registerHit(PlayerAttributes& playerAttributes, pair<int, int> coords, eShipType shipType, AttackResult& res);

	static int calculateSinkScore(char c);

	

private:
	/* input argument for playing without graphics */
	static const string PARAM_QUIET; 
	/* input argument for setting the delay between operations when playing with graphics */
	static const string PARAM_DELAY; 
	static const string BOARD_FILE_SUFFIX;
	static const string BAD_STRING; // for getDirPath validation
	static const int MAX_PATH_LEN;	

	/* sets 2 individual boards - boardA and boardB - from board*/
	static void initIndividualBoards(string *board, char **boardA, char **boardB);

	/* Checks if the ship's shape starting at board[i][j] is valid */
	static int checkShape(string* board, const int size, int i, int j);

	/* prints the various error encountered on the board (if any)*/
	static int printBoardErrors(bitset<4>& errShipsA, bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck);

	/* validates the given board*/
	static int checkBoardValidity(string* board);

	/* returns an absolute path to the current working directory */
	static string getDirPath();

	/* initializes the given PlayerAttribute struct, according to the given board*/
	static void initPlayersAttributes(PlayerAttributes& playerAttributes, const char** playerBoard);

	/* helper function - initializes the ship list in the given PlayerAttribute struct, according to the given board*/
	static void initPlayerShipsList(const char** board, int numOfRows, int numOfCols, PlayerAttributes& playerAttributes);

	/* a helper function to the initPlayerShipsList method */
	static Ship handleShipDiscovery(int iOrig, int jOrig, int numOfRows, int numOfCols, const char** board);

};
