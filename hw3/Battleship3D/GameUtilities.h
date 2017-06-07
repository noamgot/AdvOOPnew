#pragma once
#include "CommonUtilities.h"
#include "GameResultsTable.h"
#include "PlayerGameResults.h"
#include "AlgorithmLoader.h"
#include "MyBoardData.h"


// todo - consider deleting this namespace (unused)

namespace GameUtilities
{
	
	/* prints the final results of the game */
	//void printGameResults(PlayerAttributes playerAttributesArr[]);

/*	/* player initialization #1#
	bool initPlayer(IBattleshipGameAlgo*& pPlayer, int playerNum, const char** board, const string dirPath, const string dllPath, 
					PlayerAttributes playerAttributesArr[], AlgorithmLoader& dllMngr);*/

	/* the main function of the game itself! */
	//int playTheGame(IBattleshipGameAlgo* A, IBattleshipGameAlgo* B, PlayerAttributes playerAttributesArr[], const std::string* board);

	/* prints the various error encountered on the board (if any)*/
	int printBoardErrors(std::bitset<4>& errShipsA, std::bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck);

	/* helper functions for printBoardErrors */
	void printWrongSizeOrShapeError(std::bitset<4>& errShips, int& ret, const std::string player);
	void printBadShipsCountError(int shipCountA, int shipCountB);

	/* helper function - initializes the ship list in the given PlayerAttribute struct, according to the given board*/
	//void initPlayerShipsList(const char** board, int numOfRows, int numOfCols, PlayerAttributes& playerAttributes);

	/* a helper function to the initPlayerShipsList method */
	Ship handleShipDiscovery(int iOrig, int jOrig, int numOfRows, int numOfCols, const char** board);
	
}