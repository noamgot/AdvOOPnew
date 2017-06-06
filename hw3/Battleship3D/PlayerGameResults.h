#pragma once

struct PlayerGameResults
{
	int ID, wins, losses, ptsFor, ptsAgainst;
	double percentage;

	PlayerGameResults() : ID(0), wins(0), losses(0), ptsFor(0), ptsAgainst(0), percentage(0) {}
	PlayerGameResults(int ID_, int wins_, int loses_, int ptsFor_, int ptsAgainst_, double percentage_)
		: ID(ID_), wins(wins_), losses(loses_), ptsFor(ptsFor_), ptsAgainst(ptsAgainst_), percentage(percentage_) {}
	explicit PlayerGameResults(int ID_) : ID(ID_) {}

	bool operator > (const PlayerGameResults& other) const	{ return this->percentage > other.percentage; }
	PlayerGameResults& operator+=(const PlayerGameResults& rhs);
	

};
