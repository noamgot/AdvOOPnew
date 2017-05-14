#include "SmartPlayer.h"
#include "GameUtilities.h"
#include <list>
#include <ctime>


bool SmartPlayer::isNearChar(int row, int col, eSign s, Direction* dir)
{
	if(dir == nullptr)
	{
		return verifyChar(row + 1, col, s).second || verifyChar(row - 1, col, s).second || verifyChar(row, col + 1, s).second || verifyChar(row, col - 1, s).second;
	}
	bool dirs[NUMBER_OF_DIRECTIONS] = { verifyChar(row + 1, col, s).second, verifyChar(row - 1, col, s).second, verifyChar(row, col + 1, s).second, verifyChar(row, col - 1, s).second };
	for (auto i = 0; i < NUMBER_OF_DIRECTIONS; i++)
	{
		if(dirs[i])
		{
			if (i == 0 || i == 1) { *dir = VERTICAL; }
			if (i == 2 || i == 3) { *dir = HORIZONAL; }
			return true;
		}
	}
	return false;
}

bool SmartPlayer::replaceChar(int row, int col, char old_char, char new_char, bool reverse)
{
	auto pr = verifyChar(row, col, old_char);
	if (pr.second != reverse)
	{
		mBoard[row][col] = new_char;
		return true;
	}
	return false;
}

void SmartPlayer::addTargetLoop(int row, int col,int rowMod, int colMod, deque<pair<int, int>>& attackQueue, Direction dir, Direction orientation, Direction positiveDirection, Direction negativeDirection)
{
	auto maxDist = 4;
	auto foundPos = false, foundNeg = false;
	if (dir == orientation || dir == positiveDirection || dir == negativeDirection)
	{
		for (auto i = 1; i < maxDist; i++)
		{
			// Adds coordinates to the move queue only if the coordinate content is unknown 
			// and it is adjacent to a destroyed part of an enemy ship.

			//This part handles RIGHT and DOWN directions
			if (dir != positiveDirection && !foundPos && verifyChar(row + i*rowMod, col + i*colMod, UNKNOWN).second && verifyChar(row, col + i - 1, DESTROYED).second)
			{
				foundPos = true; attackQueue.push_front(make_pair(row + 1 + i*rowMod, col + 1 + i*colMod));
			}
			else if (!verifyChar(row + (i - 1)*rowMod, col + (i - 1)*colMod, DESTROYED).second) { foundPos = true; }


			// This part handles LEFT and UP directions
			if (dir != RIGHT && !foundNeg && verifyChar(row - i*rowMod, col - i*colMod, UNKNOWN).second && verifyChar(row, col - i + 1, DESTROYED).second)
			{
				foundNeg = true; attackQueue.push_front(make_pair(row + 1 - i*rowMod, col + 1 - i*colMod));
			}
			else if (!verifyChar(row - (i + 1)*rowMod, col - (i + 1)*colMod, DESTROYED).second) { foundNeg = true; }
		}
	}
}

void SmartPlayer::addTarget(int row, int col, deque<pair<int, int>>& attackQueue, Direction dir)
{
	//auto maxDist = 4;
	//auto foundUp = false, foundDown = false, foundLeft = false, foundRight = false;
	// This means we are just adding a regular target (have no additional info)
	if (dir == NONE && verifyChar(row, col, UNKNOWN).second) { attackQueue.push_front(make_pair(row + 1, col + 1)); }

	else // In this case we have some additional information
	{
		addTargetLoop(row, col, 0, 1, attackQueue, dir, HORIZONAL, RIGHT, LEFT);
		addTargetLoop(row, col, 1, 0, attackQueue, dir, VERTICAL, DOWN, UP);
	}
}

pair<bool, bool> SmartPlayer::verifyChar(int row, int col, char c)
{
	if (isPointValid(row, col))
	{
		return make_pair(true, mBoard[row][col] == c);
	}
	return make_pair(false, false);
}

void SmartPlayer::reLabelTheBoard()
{
	for (auto row = 0; row < mNumOfRows; row++)
	{
		for (auto col = 0; col < mNumOfCols; col++)
		{
			if (mBoard[row][col] != GameUtilities::eShipChar::WATER)
			{
				mBoard[row][col] = SHIP;
				mMyCoords.insert(make_pair(row, col));
			}
			else
			{
				mBoard[row][col] = UNKNOWN;
			}
		}
	}
}

void SmartPlayer::setBoard(int player, const char** board, int numRows, int numCols)
{
	AbstractPlayer::setBoard(player, board, numRows, numCols);
	reLabelTheBoard();

	for(auto row = 0; row < mNumOfRows; row++)
	{
		for(auto col = 0; col < mNumOfCols; col++)
		{
			if (isNearChar(row, col, SHIP))
			{
				replaceChar(row, col, UNKNOWN, EMPTY);	
			}
		}
	}
}

bool SmartPlayer::init(const string& path)
{
	if (!AbstractPlayer::init(path))
	{
		return false;
	}
	vector<pair<int, int>> valid_moves;
	for (auto row = 0; row < mNumOfRows; row++)
	{
		for (auto col = 0; col < mNumOfCols; col++)
		{
			if (mBoard[row][col] == UNKNOWN)
			{
				valid_moves.push_back(make_pair(row + 1,col + 1));
			}
		}
	}
	srand(static_cast<unsigned int>(time(nullptr))); // Seed the PRG for all of our random needs.

	random_shuffle(std::begin(valid_moves), std::end(valid_moves));
	for (auto move : valid_moves)
	{
		mMovesQueue.push_back(move);
	}
	return true;
}

pair<int, int> SmartPlayer::attackFromPriorityQuque(deque<pair<int, int>>& priorityQueue)
{
	auto& move = priorityQueue.front();
	while (mBoard[move.first - 1][move.second - 1] != UNKNOWN)
	{
		if (priorityQueue.size() > 0)
		{
			move = priorityQueue.front();
			priorityQueue.pop_front();
		}
		else
		{
			return make_pair(-1, -1);
		}
	}
	return move;	
}

pair<int, int> SmartPlayer::attack()
{
	if (mHighPriorityQueue.size() > 0)
	{
		auto move = attackFromPriorityQuque(mHighPriorityQueue);
		if(isPointValid(move))
		{
			return move;
		}
	}
	if (mMovesQueue.size() > 0)
	{
		auto move = attackFromPriorityQuque(mMovesQueue);
		if (isPointValid(move))
		{
			return move;
		}
	}
	return make_pair(-1,-1);
}

void SmartPlayer::analyzeAttackResult()
{
	auto dir = NONE;
	for (auto row = 0; row < mNumOfRows; row++)
	{
		for (auto col = 0; col < mNumOfCols; col++)
		{
			if(mBoard[row][col] == DESTROYED)
			{
				if (isNearChar(row, col, DESTROYED, &dir)) { addTarget(row, col, mHighPriorityQueue, dir); }
				else
				{
					if (isPointValid(row, col - 1)) { addTarget(row, col - 1, mHighPriorityQueue); }
					if (isPointValid(row - 1, col)) { addTarget(row - 1, col, mHighPriorityQueue); }
					if (isPointValid(row, col + 1)) { addTarget(row, col + 1, mHighPriorityQueue); }
					if (isPointValid(row + 1, col)) { addTarget(row + 1, col, mHighPriorityQueue); }
				}
			}
		}
	}
}

void SmartPlayer::sinkShip(int row, int col, Direction dir)
{
	//If the ship is horizonal we keep the row constant and move along the columns.
	//If the ship is not horizonal (vertical) we keep the col constant and move along the rows.
	if(dir == HORIZONAL)
	{
		for (auto i = 0; i < SHIP_MAX_LENGTH; i++) { replaceChar(row, col + i, DESTROYED, SANK); }
		for (auto i = 0; i < SHIP_MAX_LENGTH; i++) { replaceChar(row, col - i, DESTROYED, SANK); }
	}
	else
	{
		for (auto i = 0; i < SHIP_MAX_LENGTH; i++) { replaceChar(row + i, col, DESTROYED, SANK); }
		for (auto i = 0; i < SHIP_MAX_LENGTH; i++) { replaceChar(row - i, col, DESTROYED, SANK); }
	}
}

void SmartPlayer::sinkShip(int row, int col)
{
	if (!findDirection(row, col, false))
	{
		replaceChar(row, col, DESTROYED, SANK);
	}
}

void SmartPlayer::outlineLoop(int row, int col, int rowMod, int colMod, bool reverse)
{
	auto i = 0;
	for (; verifyChar(row + (reverse ? -1 : 1) * i * rowMod, col + (reverse ? -1 : 1) * i * colMod, SANK).second && i < SHIP_MAX_LENGTH; i++)
	{
		replaceChar(row + (reverse ? -1 : 1) * i * rowMod + 1 * colMod, col + (reverse ? -1 : 1) * i * colMod + 1 * rowMod, UNKNOWN, EMPTY);
		replaceChar(row + (reverse ? -1 : 1) * i * rowMod - 1 * colMod, col + (reverse ? -1 : 1) * i * colMod - 1 * rowMod, UNKNOWN, EMPTY);
	}
	replaceChar(row + (reverse ? -1 : 1) * i * rowMod, col + (reverse ? -1 : 1) * i * colMod, UNKNOWN, EMPTY);
}

bool SmartPlayer::findDirection(int row, int col, bool outline)
{
	auto label = outline ? SANK : DESTROYED;
	if (verifyChar(row, col + 1, label).second || verifyChar(row, col - 1, label).second)
	{
		outline ? outlineSunkenEnemyShips(row, col, HORIZONAL) : sinkShip(row, col, HORIZONAL);
	}
	else if (verifyChar(row + 1, col, label).second || verifyChar(row - 1, col, label).second)
	{
		outline ? outlineSunkenEnemyShips(row, col, VERTICAL) : sinkShip(row, col, VERTICAL);
	}
	else
	{
		return false;
	}
	return true;
}

void SmartPlayer::outlineSunkenEnemyShips(int row, int col, Direction dir)
{ 
	if (dir == HORIZONAL)
	{
		outlineLoop(row, col, 0, 1, false);
		outlineLoop(row, col, 0, 1, true);
	}
	else
	{
		outlineLoop(row, col, 1, 0, false);
		outlineLoop(row, col, 1, 0, true);
	}
}

void SmartPlayer::outlineSunkenEnemyShips(int row, int col)
{
	if (!findDirection(row, col, true))
	{
		replaceChar(row + 1, col, UNKNOWN, EMPTY);
		replaceChar(row - 1, col, UNKNOWN, EMPTY);
		replaceChar(row, col + 1, UNKNOWN, EMPTY);
		replaceChar(row, col - 1, UNKNOWN, EMPTY);
	}
}

void SmartPlayer::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (!GameUtilities::isLegalMove(row, col, mNumOfRows, mNumOfCols)) // ignore invalid moves
	{
		return;
	}
	auto myRow = row - 1, myCol = col - 1;
	if (result != AttackResult::Miss && verifyChar(myRow, myCol, UNKNOWN).second)
	{
		replaceChar(myRow, myCol, UNKNOWN, DESTROYED);
		if (result == AttackResult::Hit)
		{
			analyzeAttackResult();
		}
		else 
		{
			sinkShip(myRow, myCol);
			outlineSunkenEnemyShips(myRow, myCol);
		}
	}
	else
	{
		replaceChar(myRow, myCol, UNKNOWN, EMPTY);
	}	
}

IBattleshipGameAlgo* GetAlgorithm()
{
	IBattleshipGameAlgo *newP = new SmartPlayer;
	return newP;
}
