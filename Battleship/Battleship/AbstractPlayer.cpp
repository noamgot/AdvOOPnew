#include "AbstractPlayer.h"

AbstractPlayer::AbstractPlayer() : mShipsCount(DEFAULT_SHIPS_COUNT)
{
	// zero the board
	memset(this->mBoard, 0, sizeof(char)*ROW_SIZE*COL_SIZE);
}

AbstractPlayer::~AbstractPlayer()
{
}

void AbstractPlayer::setBoard(int player, const char **board, int numRows, int numCols)
{
	this->mPlayerNum = player;
	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < numCols; ++j)
		{
			this->mBoard[i][j] = board[i][j];
		}
	}

}

bool AbstractPlayer::init(const std::string & path)
{
	return true;
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

bool AbstractPlayer::registerHit(std::pair<int,int> coords, eShipType shipType, AttackResult& res)
{
	int i = 0;
	bool validAttack = false;
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




Ship AbstractPlayer::handleShipDiscovery(int iOrig, int jOrig, const char board[][COL_SIZE])
{
	int i = iOrig;
	int j = jOrig;
	int size = 0;

	std::map<std::pair<int,int>, bool> coordinates;
	char c = board[i][j];
	// we will iterate only downwards or rightwards
	do
	{
		// remember that we save the coordinates from in the form of 1 to ROW/COL SIZE (and not starting from 0)
		// hence we give a +1 offset
		coordinates[make_pair(i+1, j+1)] = true;
		size++;
	}
	while(++i < ROW_SIZE && board[i][j] == c); // checking downwards
	i = iOrig;
	while (++j < COL_SIZE && board[i][j] == c) // checking rightwards
	{
		coordinates[make_pair(i+1, j+1)] = true;
		size++;
	}
	eShipType type = Ship::charToShipType(c);
	return Ship(size, type, coordinates);
}

void AbstractPlayer::initShipsList()
{
	Ship ship;
	char c;
	for (int i = 0; i < ROW_SIZE; ++i)
	{
		for (int j = 0; j < COL_SIZE; ++j)
		{
			c = this->mBoard[i][j];
			if (c != WATER)
			{
				if ((i > 0 && mBoard[i-1][j] == c) || (j > 0 && mBoard[i][j-1] == c)) // already encountered this ship
				{
					continue;
				}
				ship = handleShipDiscovery(i,j, this->mBoard);
				this->mShipsList.push_back(ship);
			}
		}
	}
}

