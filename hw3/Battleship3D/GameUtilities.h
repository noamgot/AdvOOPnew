#pragma once
#include "CommonUtilities.h"
#include "GameResultsTable.h"
#include "PlayerGameResults.h"
#include "AlgorithmLoader.h"
#include "MyBoardData.h"


// todo - consider deleting this namespace (unused)

namespace GameUtilities
{

	/* prints the various error encountered on the board (if any)*/
	int printBoardErrors(std::bitset<4>& errShipsA, std::bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck);

	/* helper functions for printBoardErrors */
	void printWrongSizeOrShapeError(std::bitset<4>& errShips, int& ret, const std::string player);
	void printBadShipsCountError(int shipCountA, int shipCountB);

	/* a helper function to the initPlayerShipsList method */
	Ship handleShipDiscovery(int iOrig, int jOrig, int numOfRows, int numOfCols, const char** board);
	
}