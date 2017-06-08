#pragma once

struct PlayerGameResults
{
	int ID, wins, losses, ptsFor, ptsAgainst;
	double percentage;

	PlayerGameResults() : ID(0), wins(0), losses(0), ptsFor(0), ptsAgainst(0), percentage(0) {}
	PlayerGameResults(int ID_, int wins_, int loses_, int ptsFor_, int ptsAgainst_, double percentage_)
		: ID(ID_), wins(wins_), losses(loses_), ptsFor(ptsFor_), ptsAgainst(ptsAgainst_), percentage(percentage_) {}

	bool operator > (const PlayerGameResults& other) const	
	{ 
		if (this->percentage != other.percentage)
		{
			return this->percentage > other.percentage;
		}
		if (this->ptsFor != other.ptsFor)
		{
			return this->ptsFor > other.ptsFor;
		}
		return this->ptsAgainst < other.ptsAgainst;
	}
	
	PlayerGameResults& operator+=(const PlayerGameResults& rhs)
	{
		this->wins += rhs.wins;
		this->losses += rhs.losses;
		this->ptsFor += rhs.ptsFor;
		this->ptsAgainst += rhs.ptsAgainst;

		// the "true" case happens only if the player alwayes tied..
		this->percentage = (0 == wins + losses) ? 0 : 100.0 * wins / (wins + losses);
		return *this;
	}
	

};
