
#include "GameUtilities.h"


eShipType charToShipType(char c)
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

int calculateSinkScore(char c)
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

void changeCurrentPlayer(int *attackerNum, int *defenderNum)
{
	*attackerNum = *attackerNum ? 0 : 1;
	*defenderNum = *defenderNum ? 0 : 1;
}
