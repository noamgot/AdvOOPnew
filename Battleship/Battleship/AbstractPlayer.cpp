#include "AbstractPlayer.h"

AbstractPlayer::AbstractPlayer() : mShipsCount(DEFAULT_SHIPS_COUNT) {}

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

bool AbstractPlayer::init(const std::string & path)
{
	return true;
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

}

bool AbstractPlayer::hasMoves() const
{
	return !mMovesQueue.empty();
}

//char ** AbstractPlayer::getBoard()
//{
//    char **retBoard = new char *[ROW_SIZE];
//    for (int i = 0; i < ROW_SIZE; ++i)
//    {
//        retBoard[i] = new char[COL_SIZE];
//        for (int j = 0; j < COL_SIZE; ++j)
//        {
//            retBoard[i][j] = this->board[i][j];
//        }
//    }
//    return retBoard;
//}

bool AbstractPlayer::registerHit(std::pair<int, int> coords, eShipType shipType, AttackResult& res)
{
	auto i = 0;
	auto validAttack = false;
	for(; i < DEFAULT_SHIPS_COUNT; i++)
	{
		if(this->mShipsList[i].getType() == shipType)
		{
			//Make sure this coordinate belongs to this ship
			if(this->mShipsList[i].getCoordinates().count(coords) == 1)
			{
				validAttack = this->mShipsList[i].handleHit(coords, res);
				if(res == AttackResult::Sink)
				{
					this->mShipsCount--;
				}
				break;
			}
		}
	}
	return validAttack;
}

bool AbstractPlayer::hasShips() const
{
	return (this->mShipsCount > 0);
}




Ship AbstractPlayer::handleShipDiscovery(int iOrig, int jOrig, std::vector<std::vector<char>>& board)
{
	auto i = iOrig;
	auto j = jOrig;
	auto size = 0;
	std::map<std::pair<int,int>, bool> coordinates;
	auto c = board[i][j];
	// we will iterate only downwards or rightwards
	do
	{
		// remember that we save the coordinates from in the form of 1 to ROW/COL SIZE (and not starting from 0)
		// hence we give a +1 offset
		coordinates[std::make_pair(i+1, j+1)] = true;
		size++;
	}
	while(++i < ROW_SIZE && board[i][j] == c); // checking downwards
	i = iOrig;
	while (++j < COL_SIZE && board[i][j] == c) // checking rightwards
	{
		coordinates[std::make_pair(i+1, j+1)] = true;
		size++;
	}
	eShipType type = Ship::charToShipType(c);
	return Ship(size, type, coordinates);
}

void AbstractPlayer::initShipsList()
{
	auto rows = this->mNumOfRows;
	auto cols = this->mNumOfCols;
	for (auto i = 0; i < rows; ++i)
	{
		for (auto j = 0; j < cols; ++j)
		{
			auto c = this->mBoard[i][j];
			if (c != WATER)
			{
				if ((i > 0 && mBoard[i-1][j] == c) || (j > 0 && mBoard[i][j-1] == c)) // already encountered this ship
				{
					continue;
				}
				auto ship = handleShipDiscovery(i,j, this->mBoard);
				this->mShipsList.push_back(ship);
			}
		}
	}
}

