#include "NaivePlayer.h"

bool NaivePlayer::allowedToAttack(int i, int j)
{
	if (this->board[i][j] == WATER)
	{
		if (i > 0 && i < COL_SIZE-1 && j > 0 && j < COL_SIZE-1)
		{
			if (this->board[i-1][j] == WATER && this->board[i][j-1] == WATER
				&& this->board[i][j+1] == WATER && this->board[i+1][j] == WATER)
			{
				return true;
			}
		}
				
	}
	return false;
}

void NaivePlayer::initAttackQueue()
{
	for (int i = 0; i < ROW_SIZE; i++)
	{
		for (int j = 0; j < COL_SIZE; j++)
		{
			if (allowedToAttack(i, j))
			{
				this->movesQueue.push(make_pair(i, j));
			}
		}
	}

}



