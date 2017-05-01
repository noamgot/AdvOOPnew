#include "AbstractPlayer.h"
#include "GameUtilities.h"
#include <iostream>

AbstractPlayer::AbstractPlayer() : mShipsCount(GameUtilities::DEFAULT_SHIPS_COUNT) {}

AbstractPlayer::~AbstractPlayer() {}


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
std::pair<int, int> AbstractPlayer::attack()
{
	if (mMovesQueue.size() > 0)
	{
		auto& nextAttack(mMovesQueue.front());
		mMovesQueue.pop();
		return nextAttack;
	}
	return std::make_pair(-1, -1);
}

void AbstractPlayer::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (!GameUtilities::isLegalMove(make_pair(row,col), mNumOfRows, mNumOfCols))
	{
		cout << "Error: Invalid move from player " << player << " - (" << row << "," << col << ")" << endl;
	}
}

bool AbstractPlayer::init(const std::string& path)
{
	// validate path
	if (!GameUtilities::isValidPath(path))
	{
		cout << "Error: failed to init player - got invalid path: " << path << endl;
		return false;
	}
	return true;
}

bool AbstractPlayer::hasMoves() const
{
	return !mMovesQueue.empty();
}



bool AbstractPlayer::hasShips() const
{
	return (this->mShipsCount > 0);
}