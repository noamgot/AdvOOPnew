#include "SmartPlayer.h"
#include "GameUtilities.h"


SmartPlayer::SmartPlayer()
{
}


SmartPlayer::~SmartPlayer()
{
}

void SmartPlayer::setBoard(int player, const char** board, int numRows, int numCols)
{
	int spaceIOccupy = 0;
	float initialWeight;
	AbstractPlayer::setBoard(player, board, numRows, numCols);
	for each (Ship s in mShipsList)
	{
		spaceIOccupy += s.getSize();
	}
	initialWeight = float(1.0 / spaceIOccupy);
	for(int row = 0; row < ROW_SIZE; row++)
	{
		for(int col = 0; col < COL_SIZE; col++)
		{
			mWeightedBoard[row][col].setPosition(row + 1, col + 1);
			if (mBoard[row][col] == WATER && !isNearAShip(row,col))
			{
				mWeightedBoard[row][col].setWeight(initialWeight); //@ALL: Should change the signature to rvalue ref? (?)_(?)
			}
			else
			{
				mWeightedBoard[row][col].setWeight(0);
			}
		}
	}
}

bool SmartPlayer::init(const std::string& path)
{
	AbstractPlayer::init(path);
	for (int row = 0; row < ROW_SIZE; row++)
	{
		for (int col = 0; col < COL_SIZE; col++)
		{
			if (mWeightedBoard[row][col].getWeight() > 0)
			{
				mValidMoves.push_back(mWeightedBoard[row][col]);
			}
		}
	}
	std::random_shuffle(mValidMoves.begin(), mValidMoves.end());

	return true;
}

bool SmartPlayer::hasMoves() const
{
	return (mValidMoves.size() > 0);
}


std::pair<int, int> SmartPlayer::attack()
{
	createPriorityQueue();
	if (mWeightedMovesQueue.size() > 0)
	{
		WeightedPosition nextAttack(mWeightedMovesQueue.top());
		mWeightedMovesQueue.pop();
		return nextAttack.getPos();
	}
	return std::make_pair(-1, -1);
}

void SmartPlayer::smartSetWeight(int row, int col, float val)
{
	if(!isNearAShip(row, col) && row >= 0 && row < ROW_SIZE && col >= 0 && col < COL_SIZE)
	{
		mWeightedBoard[row][col].setWeight(val);
	}
}

//TODO - any spot that gets assigned 0 prob should be remvoed from the mValidMoves map
void SmartPlayer::updateWeights(int row, int col, AttackResult res)
{
	Direction dir = Direction::NONE;
	smartSetWeight(row, col, 0);

	if(res != AttackResult::Miss)
	{
		if (mBoard[row][col + 1] == 'X' || mBoard[row][col - 1] == 'X')
		{
			updateWeightArounHit(row, col, Direction::HORIZONAL,res);
		}
		else if (mBoard[row + 1][col] == 'X' || mBoard[row - 1][col] == 'X')
		{
			updateWeightArounHit(row, col, Direction::VERTICAL, res);
		}
		else
		{
			//Boat
			if (row + 1 > ROW_SIZE) { smartSetWeight(row + 1, col, 0.25); }
			if (row - 1 > 0)		{ smartSetWeight(row - 1, col, 0.25); }
			if (col + 1 < COL_SIZE) { smartSetWeight(row , col + 1, 0.25); }
			if (col + 1 < COL_SIZE) { smartSetWeight(row, col - 1, 0.25); }
		}
	}	
}

void SmartPlayer::updateWeightArounHit(int row, int col, Direction dir, AttackResult res)
{
	if(dir == Direction::HORIZONAL || dir == Direction::RIGHT || dir == Direction::LEFT)
	{
		if (dir != Direction::LEFT && col + 1 < COL_SIZE && mBoard[row][col + 1] == 'X')
		{
			updateWeightArounHit(row, col + 1, Direction::RIGHT, res);

			smartSetWeight(row + 1, col, 0);
			smartSetWeight(row + 1, col + 1, 0);

			smartSetWeight(row - 1, col, 0);
			smartSetWeight(row - 1, col + 1, 0);
		}
		else if(dir != Direction::LEFT && col + 1 < COL_SIZE && mBoard[row][col + 1] == WATER && res == AttackResult::Hit)
		{
			smartSetWeight(row, col + 1, 0.5);
		}
		
		if (dir != Direction::RIGHT && col - 1 >= 0 && mBoard[row][col - 1] == 'X')
		{
			updateWeightArounHit(row, col - 1, Direction::LEFT, res);

			smartSetWeight(row + 1, col, 0);
			smartSetWeight(row + 1, col - 1, 0);
			
			smartSetWeight(row - 1, col, 0);
			smartSetWeight(row - 1, col - 1, 0);
		}
		else if (dir != Direction::RIGHT && col - 1 >= 0 && mBoard[row][col + 1] == WATER && res == AttackResult::Hit)
		{
			smartSetWeight(row, col - 1, 0.5);
		}
	}

	else if (dir == Direction::VERTICAL || dir == Direction::UP || dir == Direction::DOWN)
	{
		if (dir != Direction::DOWN  && row + 1 < ROW_SIZE && mBoard[row + 1][col] == 'X')
		{
			updateWeightArounHit(row + 1, col, Direction::UP, res);

			smartSetWeight(row, col + 1, 0);
			smartSetWeight(row + 1, col + 1, 0);

			smartSetWeight(row, col - 1, 0);
			smartSetWeight(row + 1, col - 1, 0);
		}
		else if (dir != Direction::DOWN  && row + 1 < ROW_SIZE && mBoard[row + 1][col] == WATER && res == AttackResult::Hit)
		{
			smartSetWeight(row + 1, col, 0.5);
		}
		if (dir != Direction::UP  && row - 1 >= 0 && mBoard[row - 1][col] == 'X')
		{
			updateWeightArounHit(row + 1, col, Direction::UP, res);

			smartSetWeight(row, col + 1, 0);
			smartSetWeight(row - 1, col + 1, 0);

			smartSetWeight(row, col - 1, 0);
			smartSetWeight(row - 1, col - 1, 0);
		}
		if (dir != Direction::UP  && row - 1 >= 0 && mBoard[row - 1][col] == WATER && res == AttackResult::Hit)
		{
			smartSetWeight(row - 1, col, 0.5);
		}
	}
}

void SmartPlayer::createPriorityQueue()
{
	
}


void SmartPlayer::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	int myRow = row - 1, myCol = col - 1;
	
	if (result != AttackResult::Miss && mBoard[myRow][myCol] == WATER)
	{
		//TODO - replace the 'X' with something nicer 
		mBoard[myRow][myCol] = 'X'; //X means destroyed something
	}
	else
	{
		//TODO - replace the 'O' with something nicer 
		mBoard[myRow][myCol] = 'O'; //O means empty
	}
	
	updateWeights(myRow, myCol,result);
}


bool SmartPlayer::isNearAShip(int row, int col)
{
	return Ship::charToShipType(mBoard[row + 1][col]) != eShipType::SHIP_TYPE_ERROR || Ship::charToShipType(mBoard[row - 1][col]) != eShipType::SHIP_TYPE_ERROR ||
		Ship::charToShipType(mBoard[row][col + 1]) != eShipType::SHIP_TYPE_ERROR || Ship::charToShipType(mBoard[row][col - 1]) != eShipType::SHIP_TYPE_ERROR;
}
