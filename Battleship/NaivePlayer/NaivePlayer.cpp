#include "NaivePlayer.h"

bool NaivePlayer::allowedToAttack(int i, int j) const
{
	auto up = true, down = true, right = true, left = true;
	auto rows = this->mNumOfRows;
	auto cols = this->mNumOfCols;
	char water = GameUtilities::eShipChar::WATER;
	if (this->mBoard[i][j] != water) // do not self-hit ships
	{
		return false;
	}
	// update side booleans if possible
	if (i > 0)
	{
		up = this->mBoard[i - 1][j] == water;
	}
	if (i < rows-1)
	{
		down = this->mBoard[i + 1][j] == water;
	}
	if (j > 0)
	{
		left = this->mBoard[i][j - 1] == water;
	}
	if (j < cols-1)
	{
		right = this->mBoard[i][j + 1] == water;
	}
	// return true iff all sides are water (i.e clear to attack)
	return up && down && right && left;
}

bool NaivePlayer::init(const string& path)
{
	if (!AbstractPlayer::init(path))
	{
		return false;
	}
	// init moves queue
	for (auto i = 0; i < mNumOfRows; i++)
	{
		for (auto j = 0; j < mNumOfCols; j++)
		{
			if (allowedToAttack(i, j))
			{
				// we give a (+1) offset so we "speak" in cordinates starting from 1
				mMovesQueue.push(make_pair(i + 1, j + 1));
			}
		}
	}
	return true;
}

IBattleshipGameAlgo* GetAlgorithm()
{
	IBattleshipGameAlgo *newP = new NaivePlayer;
	return newP;
}

