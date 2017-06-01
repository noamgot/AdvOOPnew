#include "SmartPlayer.h"


bool SmartPlayer::isNearChar(int row, int col, int depth, eSign s, Direction* dir)
{
	if(dir == nullptr)
	{
		return verifyChar(row + 1, col, depth, s).second || verifyChar(row - 1, col, depth, s).second || verifyChar(row, col + 1, depth, s).second
			|| verifyChar(row, col - 1, depth, s).second || verifyChar(row, col, depth - 1, s).second || verifyChar(row, col, depth + 1, s).second;
	}
	bool dirs[NUMBER_OF_DIRECTIONS] = { verifyChar(row + 1, col, depth, s).second, verifyChar(row - 1, col, depth, s).second,
										verifyChar(row, col + 1, depth, s).second, verifyChar(row, col - 1, depth, s).second,
										verifyChar(row, col, depth + 1, s).second, verifyChar(row, col, depth - 1, s).second };
	for (auto i = 0; i < NUMBER_OF_DIRECTIONS; i++)
	{
		if(dirs[i])
		{
			if (i == 0 || i == 1) { *dir = VERTICAL; }
			if (i == 2 || i == 3) { *dir = HORIZONAL; }
			if (i == 4 || i == 5) { *dir = DEPTH; }
			return true;
		}
	}
	return false;
}

bool SmartPlayer::replaceChar(int row, int col, int depth, char old_char, char new_char, bool reverse)
{
	auto pr = verifyChar(row, col, depth,  old_char);
	if (pr.second != reverse)
	{
		mBoard.setChar(Coordinate(row, col, depth), new_char);
		return true;
	}
	return false;
}

void SmartPlayer::addTargetLoop(int row, int col, int depth, int rowMod, int colMod, int depthMod, deque<Coordinate>& attackQueue, Direction dir, Direction orientation, Direction positiveDirection, Direction negativeDirection)
{
	auto maxDist = 4;
	auto foundPos = false, foundNeg = false;
	if (dir == orientation || dir == positiveDirection || dir == negativeDirection)
	{
		for (auto i = 1; i < maxDist; i++)
		{
			// Adds coordinates to the move queue only if the coordinate content is unknown 
			// and it is adjacent to a destroyed part of an enemy ship.

			//This part handles RIGHT, DOWN and BOTTOM directions
			if (dir != positiveDirection && !foundPos && verifyChar(row + i*rowMod, col + i*colMod, depth + i*depthMod, UNKNOWN).second && verifyChar(row + (i - 1)*rowMod, col + (i - 1)*colMod, depth + (i - 1)*depthMod, DESTROYED).second)
			{
				foundPos = true; attackQueue.push_front(Coordinate(row + 1 + i*rowMod, col + 1 + i*colMod, depth + 1 + i*depthMod));
			}
			else if (!verifyChar(row + (i - 1)*rowMod, col + (i - 1)*colMod, depth + (i - 1)*depthMod, DESTROYED).second) { foundPos = true; }


			// This part handles LEFT, UP and TOP directions
			if (dir != RIGHT && !foundNeg && verifyChar(row - i*rowMod, col - i*colMod, depth - i*depthMod, UNKNOWN).second && verifyChar(row - (i - 1)*rowMod, col - (i - 1)*colMod, depth - (i - 1)*depthMod, DESTROYED).second)
			{
				foundNeg = true; attackQueue.push_front(Coordinate(row - (i + 1)*rowMod, col - (i + 1)*colMod, depth - (i + 1)*depthMod));
			}
			else if (!verifyChar(row - (i + 1)*rowMod, col - (i + 1)*colMod, depth - (i + 1)*depthMod, DESTROYED).second) { foundNeg = true; }
		}
	}
}

void SmartPlayer::addTarget(int row, int col, int depth, deque<Coordinate>& attackQueue, Direction dir)
{
	//auto maxDist = 4;
	//auto foundUp = false, foundDown = false, foundLeft = false, foundRight = false;
	// This means we are just adding a regular target (have no additional info)
	if (dir == NONE && verifyChar(row, col, depth, UNKNOWN).second) { attackQueue.push_front(Coordinate(row + 1, col + 1, depth + 1)); }

	else // In this case we have some additional information
	{
		addTargetLoop(row, col, depth, 0, 1, 0, attackQueue, dir, HORIZONAL, RIGHT, LEFT);
		addTargetLoop(row, col, depth, 1, 0, 0, attackQueue, dir, VERTICAL, DOWN, UP);
		addTargetLoop(row, col, depth, 0, 0, 1, attackQueue, dir, DEPTH, BOTTOM, TOP);
	}
}

pair<bool, bool> SmartPlayer::verifyChar(int row, int col, int depth, char c)
{
	Coordinate cord(row, col, depth);
	if (isPointValid(cord))
	{
		return make_pair(true, mBoard.charAt(cord) == c);
	}
	return make_pair(false, false);
}

void SmartPlayer::reLabelTheBoard()
{
	for (auto row = 0; row < mRows; row++)
	{
		for (auto col = 0; col < mCols; col++)
		{
			for (auto depth = 0; depth < mDepth; ++depth)
			{
				Coordinate cord(row, col, depth);
				if (mBoard.charAt(cord) != GameUtilities::eShipChar::WATER)
				{
					mBoard.setChar(cord, SHIP);
					mMyCoords.insert(cord);
				}
				else
				{
					mBoard.setChar(cord, UNKNOWN);
				}
			}
		}
	}
}

void SmartPlayer::setBoard(const BoardData &board)
{
	reLabelTheBoard();

	for(auto row = 0; row < mRows; row++)
	{
		for(auto col = 0; col < mCols; col++)
		{
			for (auto depth = 0; depth < mDepth; ++depth)
			{
				if (isNearChar(row, col, depth, SHIP))
				{
					replaceChar(row, col, depth, UNKNOWN, EMPTY);
				}
			}
		}
	}
}

bool SmartPlayer::init()
{
	vector<Coordinate> valid_moves;
	for (auto row = 0; row < mRows; row++)
	{
		for (auto col = 0; col < mCols; col++)
		{
			for (auto depth = 0; depth < mDepth; depth++)
			{
				Coordinate cord(row, col, depth);
				if (mBoard.charAt(cord) == UNKNOWN)
				{
					valid_moves.push_back(Coordinate(row + 1, col + 1, depth + 1));
				}
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

Coordinate SmartPlayer::attackFromPriorityQuque(deque<Coordinate>& priorityQueue)
{
	auto& move = priorityQueue.front();
	while (mBoard.charAt(Coordinate(move.row, move.col, move.depth)) != UNKNOWN)
	{
		if (priorityQueue.size() > 0)
		{
			move = priorityQueue.front();
			priorityQueue.pop_front();
		}
		else
		{
			return Coordinate(-1, -1, -1);
		}
	}
	return move;	
}

Coordinate SmartPlayer::attack()
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
	return Coordinate(-1, -1, -1);
}

void SmartPlayer::analyzeAttackResult()
{
	auto dir = NONE;
	for (auto row = 0; row < mRows; row++)
	{
		for (auto col = 0; col < mCols; col++)
		{
			for (auto depth = 0; depth < mDepth; ++depth)
			{
				Coordinate cord(row, col, depth);
				if (mBoard.charAt(cord) == DESTROYED)
				{
					if (isNearChar(row, col, depth, DESTROYED, &dir)) { addTarget(row, col, depth, mHighPriorityQueue, dir); }
					else
					{
						if (isPointValid(row, col - 1, depth)) { addTarget(row, col - 1, depth, mHighPriorityQueue); }
						if (isPointValid(row - 1, col, depth)) { addTarget(row - 1, col, depth, mHighPriorityQueue); }
						if (isPointValid(row, col + 1, depth)) { addTarget(row, col + 1, depth, mHighPriorityQueue); }
						if (isPointValid(row + 1, col, depth)) { addTarget(row + 1, col, depth, mHighPriorityQueue); }
						if (isPointValid(row, col, depth + 1)) { addTarget(row, col, depth + 1, mHighPriorityQueue); }
						if (isPointValid(row, col, depth - 1)) { addTarget(row, col, depth - 1, mHighPriorityQueue); }
					}
				}
			}
		}
	}
}

void SmartPlayer::sinkShip(int row, int col, int depth, Direction dir)
{
	//If the ship is horizonal we keep the row constant and move along the columns.
	//If the ship is not horizonal (vertical) we keep the col constant and move along the rows.
	if(dir == HORIZONAL)
	{
		for (auto i = 0; i < SHIP_MAX_LENGTH; i++) { replaceChar(row, col + i, depth, DESTROYED, SANK); }
		for (auto i = 0; i < SHIP_MAX_LENGTH; i++) { replaceChar(row, col - i, depth, DESTROYED, SANK); }
	}
	else if (dir == VERTICAL)
	{
		for (auto i = 0; i < SHIP_MAX_LENGTH; i++) { replaceChar(row + i, col, depth, DESTROYED, SANK); }
		for (auto i = 0; i < SHIP_MAX_LENGTH; i++) { replaceChar(row - i, col, depth, DESTROYED, SANK); }
	}
	else
	{
		for (auto i = 0; i < SHIP_MAX_LENGTH; i++) { replaceChar(row, col, depth + i, DESTROYED, SANK); }
		for (auto i = 0; i < SHIP_MAX_LENGTH; i++) { replaceChar(row, col, depth - i, DESTROYED, SANK); }
	}
}

void SmartPlayer::sinkShip(int row, int col, int depth)
{
	if (!findDirection(row, col, depth, false))
	{
		replaceChar(row, col, depth, DESTROYED, SANK);
	}
}

void SmartPlayer::outlineLoop(int row, int col, int depth, int rowMod, int colMod, int depthMod, bool reverse)
{
	auto i = 0;
	for (; verifyChar(row + (reverse ? -1 : 1) * i * rowMod, col + (reverse ? -1 : 1) * i * colMod, depth + (reverse ? -1 : 1) * i * depthMod, SANK).second && i < SHIP_MAX_LENGTH; i++)
	{
		replaceChar(row + (reverse ? -1 : 1) * i * rowMod + 1 * colMod * depthMod, col + (reverse ? -1 : 1) * i * colMod + 1 * rowMod * depthMod, depth + (reverse ? -1 : 1) * i * depthMod + 1 * colMod * rowMod, UNKNOWN, EMPTY);
		replaceChar(row + (reverse ? -1 : 1) * i * rowMod - 1 * colMod * depthMod, col + (reverse ? -1 : 1) * i * colMod - 1 * rowMod * depthMod, depth + (reverse ? -1 : 1) * i * depthMod - 1 * colMod * rowMod, UNKNOWN, EMPTY);
	}
	replaceChar(row + (reverse ? -1 : 1) * i * rowMod, col + (reverse ? -1 : 1) * i * colMod, depth + (reverse ? -1 : 1) * i * depthMod, UNKNOWN, EMPTY);
}

bool SmartPlayer::findDirection(int row, int col, int depth, bool outline)
{
	auto label = outline ? SANK : DESTROYED;
	if (verifyChar(row, col + 1, depth, label).second || verifyChar(row, col - 1, depth, label).second)
	{
		outline ? outlineSunkenEnemyShips(row, col, depth, HORIZONAL) : sinkShip(row, col, depth, HORIZONAL);
	}
	else if (verifyChar(row + 1, col, depth, label).second || verifyChar(row - 1, col, depth, label).second)
	{
		outline ? outlineSunkenEnemyShips(row, col, depth, VERTICAL) : sinkShip(row, col, depth, VERTICAL);
	}
	else if (verifyChar(row, col, depth + 1, label).second || verifyChar(row, col, depth - 1, label).second)
	{
		outline ? outlineSunkenEnemyShips(row, col, depth, DEPTH) : sinkShip(row, col, depth, DEPTH);
	}
	else
	{
		return false;
	}
	return true;
}

void SmartPlayer::outlineSunkenEnemyShips(int row, int col, int depth, Direction dir)
{ 
	if (dir == HORIZONAL)
	{
		outlineLoop(row, col, depth, 0, 1, 0, false);
		outlineLoop(row, col, depth, 0, 1, 0, true);
	}
	else if (dir == VERTICAL)
	{
		outlineLoop(row, col, depth, 1, 0, 0, false);
		outlineLoop(row, col, depth, 1, 0, 0, true);
	}
	else
	{
		outlineLoop(row, col, depth, 0, 0, 1, false);
		outlineLoop(row, col, depth, 0, 0, 1, true);
	}
}

void SmartPlayer::outlineSunkenEnemyShips(int row, int col, int depth)
{
	if (!findDirection(row, col, depth, true))
	{
		replaceChar(row + 1, col, depth, UNKNOWN, EMPTY);
		replaceChar(row - 1, col, depth, UNKNOWN, EMPTY);
		replaceChar(row, col + 1, depth, UNKNOWN, EMPTY);
		replaceChar(row, col - 1, depth, UNKNOWN, EMPTY);
		replaceChar(row, col, depth + 1, UNKNOWN, EMPTY);
		replaceChar(row, col, depth - 1, UNKNOWN, EMPTY);
	}
}

void SmartPlayer::notifyOnAttackResult(int player, Coordinate move, AttackResult result)
{
	if (!GameUtilities::isLegalMove(move, mRows, mCols, mDepth)) // ignore invalid moves
	{
		return;
	}
	auto myRow = move.row - 1, myCol = move.col - 1, myDep = move.depth - 1;
	if (result != AttackResult::Miss && verifyChar(myRow, myCol, myDep, UNKNOWN).second)
	{
		replaceChar(myRow, myCol, myDep, UNKNOWN, DESTROYED);
		if (result == AttackResult::Hit)
		{
			analyzeAttackResult();
		}
		else 
		{
			sinkShip(myRow, myCol, myDep);
			outlineSunkenEnemyShips(myRow, myCol, myDep);
		}
	}
	else
	{
		replaceChar(myRow, myCol, myDep, UNKNOWN, EMPTY);
	}	
}

void SmartPlayer::setPlayer(int player)
{
	mPlayerNum = player;
}


IBattleshipGameAlgo* GetAlgorithm()
{
	IBattleshipGameAlgo* newP = new SmartPlayer;
	return newP;
}
