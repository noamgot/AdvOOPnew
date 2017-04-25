#include "NaivePlayer.h"

bool NaivePlayer::allowedToAttack(int i, int j) const
{
	bool up = true, down = true, right = true, left = true;
	if (this->mBoard[i][j] != WATER) // do not self-hit ships
	{
		return false;
	}
	// update side booleans if possible
	if (i > 0)
	{
		up = (this->mBoard[i - 1][j] == WATER);
	}
	if (i < ROW_SIZE-1)
	{
		down = (this->mBoard[i + 1][j] == WATER);
	}
	if (j > 0)
	{
		left = (this->mBoard[i][j - 1] == WATER);
	}
	if (j < COL_SIZE-1)
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
				this->mMovesQueue.push(make_pair(i, j));
			}
		}
	}

}



