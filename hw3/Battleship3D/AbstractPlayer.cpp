#include "AbstractPlayer.h"

void AbstractPlayer::setBoard(const BoardData& board)
{
	mRows = board.rows();
	mCols = board.cols();
	mDepth = board.depth();
	for (auto row = 0; row < mRows; ++row)
	{
		for (auto col = 0; col < mCols; ++col)
		{
			for (auto depth = 0; depth < mDepth; ++depth)
			{
				Coordinate coordinate = Coordinate(row, col, depth);
				mBoard.setChar(coordinate, board.charAt(coordinate));
			}			
		}
	}
}


// get next attack from the player's moves queue
Coordinate AbstractPlayer::attack()
{
	if (mMovesQueue.size() > 0)
	{
		auto& nextAttack(mMovesQueue.front());
		mMovesQueue.pop_front();
		return nextAttack;
	}
	return Coordinate(-1, -1, -1);
}

bool AbstractPlayer::init(const string& path)
{
	// validate path
	return GameUtilities::isValidPath(path);
}