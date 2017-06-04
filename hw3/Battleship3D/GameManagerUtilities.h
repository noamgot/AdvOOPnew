#pragma once
#include <bitset>
#include "GameUtilities.h"
#include "DLLManager.h"
#include "MyBoardData.h"

using namespace std;
using namespace GameUtilities;

#define PLAYER_A 0
#define PLAYER_B 1

typedef vector<vector<vector<char>>> vector3d;

namespace GameManagerUtilities
{
	/* input argument for setting the amount of threads */
	const string PARAM_THREADS("-threads");
	const string BOARD_FILE_SUFFIX(".sboard");
	const string LIB_FILE_SUFFIX(".dll");
	const string BAD_STRING("!@#"); // for getDirPath validation
	const int MAX_PATH_LEN = 1024;

	// a struct for the game manager - to hold information about the players
	typedef struct _PlayerAttributes
	{
		bool hasMoves;
		int score;
		int shipsCount;
		vector<Ship> shipList;
		bool won;
	}PlayerAttributes;

	/* initializing the game attributes, player boards and paths for the different files */
	int initGameBoardsAndPaths(int argc, char **argv, string(&board)[ROW_SIZE], char **boardA, char* *boardB,
		string& dirPath, string& dllPathA, string& dllPathB);

	int processInputArguments(int argc, char** argv, string& dirPath, int& numThreads);
	
	void changeAttacker(int& attackerNum, int& defenderNum);

	/* prints the final results of the game */
	void printGameResults(PlayerAttributes playerAttributesArr[]);

	/* Initializes the battle boards according to the .sboard file in boardPath.
	* results in a rows*cols board inside passed boards arguments - including
	* individual boards for both players									  */
	int initGameBoards(const string boardPath, string board[], char** boardA, char** boardB);

	/* player initialization */
	bool initPlayer(IBattleshipGameAlgo*& pPlayer, int playerNum, const char** board, const string dirPath, const string dllPath, 
					PlayerAttributes playerAttributesArr[], DLLManager& dllMngr);

	/* mark a ship hit to a given player, and keeping the AttackResult in res */
	bool registerHit(PlayerAttributes& playerAttributes, pair<int, int> coords, eShipType shipType, AttackResult& res);

	/* returns the amount of points according to the given ship type (by char) */
	int calculateSinkScore(char c);

	/* the main function of the game itself! */
	int playTheGame(IBattleshipGameAlgo* A, IBattleshipGameAlgo* B, PlayerAttributes playerAttributesArr[], const string* board);

	/* helper functions called inside playThe Game */
	void handleMove(const MyBoardData& board, Coordinate& move, int &attackerNum, int &defenderNum, unique_ptr<IBattleshipGameAlgo>& A,
					unique_ptr<IBattleshipGameAlgo>& B, PlayerAttributes playerAttributesArr[]);
	void handleMiss(Coordinate& move, unique_ptr<IBattleshipGameAlgo>& A, unique_ptr<IBattleshipGameAlgo>& B, int& attackerNum);
	void handleHitOrSink(Coordinate& move, bool& validAttack, unique_ptr<IBattleshipGameAlgo>& A, unique_ptr<IBattleshipGameAlgo>& B,
	                     char hitChar, int attackerNum, PlayerAttributes playerAttributesArr[]);

	/* sets 2 individual boards - boardA and boardB - from board*/
	void initIndividualBoards(const MyBoardData& board, MyBoardData& boardA, MyBoardData& boardB);

	/* Checks if the ship's shape starting at board[i][j] is valid */
	int checkShape(const MyBoardData& board, const int size, int i, int j);

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

	/* given a vector of paths to sboard files, loads only the valid ones into vector<vector3d> boards */
	void initBoards3D(const vector<string>& boardPaths, vector<vector3d>& boards);

	/* parses the dimension line (first line in .sboard file) into rows, columns and depth */
	int getDims(const string line, int& rows, int& cols, int& depth);

	/* checks if the ship's shape starting at board[i][j][k] is valid */
	int checkShape3D(vector3d& board, const int size, int k, int i, int j);

	/* checks if the passed board is valid */
	int checkBoard3D(vector3d& board);
}