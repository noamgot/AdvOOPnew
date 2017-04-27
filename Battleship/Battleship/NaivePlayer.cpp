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
	if (i < rows-1)
	{
		down = (this->mBoard[i + 1][j] == WATER);
	}
	if (j > 0)
	{
		left = (this->mBoard[i][j - 1] == WATER);
	}
	if (j < cols-1)
	{
		right = (this->mBoard[i][j + 1] == WATER);
	}
	// return true iff all sides are water (i.e clear to attack)
	return up && down && right && left;
}

void NaivePlayer::initAttackQueue()
{
	for (int i = 0; i < ROW_SIZE; i++)
	{
		for (int j = 0; j < COL_SIZE; j++)
		{
			if (allowedToAttack(i, j))
			{
				this->mMovesQueue.push(std::make_pair(i+1, j+1));
			}
		}
	}

}



