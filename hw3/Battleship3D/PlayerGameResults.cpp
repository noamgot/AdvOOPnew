#include "PlayerGameResults.h"

PlayerGameResults& PlayerGameResults::operator+=(const PlayerGameResults& rhs)
{

	this->wins += rhs.wins;
	this->losses += rhs.losses;
	this->ptsFor += rhs.ptsFor;
	this->ptsAgainst += rhs.ptsAgainst;
	this->percentage = 100.0 * wins / (wins + losses);
	return *this;
}
