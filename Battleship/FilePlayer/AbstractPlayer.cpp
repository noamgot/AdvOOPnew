#include "AbstractPlayer.h"

void AbstractPlayer::setBoard(int player, const char **board, int numRows, int numCols)
{
	mNumOfRows = numRows;
	mNumOfCols = numCols;
	mPlayerNum = player;
	// allocate memory for the player's board and copy the given board
	mBoard.resize(numRows);
	for (auto i = 0; i < numRows; ++i)
	{
		mBoard[i].resize(numCols);
		for (auto j = 0; j < numCols; ++j)
		{
			mBoard[i][j] = board[i][j];
		}
	}
}


// get next attack from the player's moves queue
pair<int, int> AbstractPlayer::attack()
{
	if (mMovesQueue.size() > 0)
	{
		auto& nextAttack(mMovesQueue.front());
		mMovesQueue.pop_front();
		return nextAttack;
	}
	return make_pair(-1, -1);
}

bool AbstractPlayer::init(const string& path)
{
	// validate path
	return GameUtilities::isValidPath(path);
}