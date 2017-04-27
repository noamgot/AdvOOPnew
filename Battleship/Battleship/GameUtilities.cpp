
#include "GameUtilities.h"
#include "Bonus.h"
#include <iostream>

using namespace std;

// initialize static consts

const string GameUtilities::PARAM_QUIET = "-quiet";
const string GameUtilities::PARAM_DELAY = "-delay";

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
		clearLastLine();
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