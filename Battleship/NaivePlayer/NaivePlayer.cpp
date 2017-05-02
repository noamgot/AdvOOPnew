#include "NaivePlayer.h"

bool NaivePlayer::allowedToAttack(int i, int j) const
{
	auto up = true, down = true, right = true, left = true;
	auto rows = this->mNumOfRows;
	auto cols = this->mNumOfCols;
	if (this->mBoard[i][j] != WATER) // do not self-hit ships
	{
		return false;
	}
	// update side booleans if possible
	if (i > 0)
	{
		up = (this->mBoard[i - 1][j] == WATER);
	}
	if (i < rows - 1)
	{
		down = (this->mBoard[i + 1][j] == WATER);
	}
	if (j > 0)
	{
		left = (this->mBoard[i][j - 1] == WATER);
	}
	if (j < cols - 1)
	{
		right = (this->mBoard[i][j + 1] == WATER);
	}
	// return true iff all sides are water (i.e clear to attack)
	return up && down && right && left;
}

bool NaivePlayer::init(const std::string& path)
{
	AbstractPlayer::init(path);

	// init moves queue
	for (auto i = 0; i < this->mNumOfRows; i++)
	{
		for (auto j = 0; j < this->mNumOfCols; j++)
		{
			if (allowedToAttack(i, j))
			{
				// we give a (+1) offset so we "speak" in cordinates starting from 1
				this->mMovesQueue.push(std::make_pair(i + 1, j + 1));
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

