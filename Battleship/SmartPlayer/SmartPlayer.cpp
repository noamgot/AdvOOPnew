#include "SmartPlayer.h"
#include "GameUtilities.h"
#include <list>


bool SmartPlayer::isNearChar(int row, int col, eSign s, Direction* dir)
{
	if(dir == NULL)
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

bool SmartPlayer::replaceChar(pair<int, int> point, char old_char, char new_char, bool reverse)
{
	return replaceChar(point.first, point.second, old_char, new_char, reverse);
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

void SmartPlayer::addTarget(int row, int col, deque<pair<int, int>>& attackQueue, Direction dir)
{
	auto maxDist = 4;
	auto foundUp = false, foundDown = false, foundLeft = false, foundRight = false;
	// This means we are just adding a regular target (have no additional info)
	if (dir == NONE && verifyChar(row, col, UNKNOWN).second) { attackQueue.push_front(make_pair(row + 1, col + 1)); }

	else // In this case we have some additional information
	{
		// TODO - code duplication here!

		// Adds coordinates to the move queue only if the coordinate content is unknown 
		// and it is adjacent to a destroyed part of an enemy ship.
		if (dir == HORIZONAL || dir == RIGHT || dir == LEFT)
		{
			for (auto i = 1; i < maxDist; i++)
			{
				if (dir != LEFT && !foundRight && verifyChar(row, col + i, UNKNOWN).second && verifyChar(row, col + i - 1, DESTROYED).second)
				{ foundRight = true; attackQueue.push_front(make_pair(row + 1, col + 1 + i)); }
				else if (!verifyChar(row, col + i - 1, DESTROYED).second) { foundRight = true; }

				if (dir != RIGHT && !foundLeft && verifyChar(row, col - i, UNKNOWN).second && verifyChar(row, col - i + 1, DESTROYED).second)
				{ foundLeft = true;attackQueue.push_front(make_pair(row + 1, col + 1 - i)); }
				else if (!verifyChar(row, col - i + 1, DESTROYED).second) { foundLeft = true; }
			}
		}

		if (dir == VERTICAL || dir == UP || dir == DOWN)
		{
			for (auto i = 1; i < maxDist; i++)
			{
				if (dir != DOWN && !foundUp && verifyChar(row - i, col, UNKNOWN).second && verifyChar(row - i + 1, col, DESTROYED).second)
				{ foundUp = true; attackQueue.push_front(make_pair(row - i + 1, col + 1)); }
				else if (!verifyChar(row - i + 1, col, DESTROYED).second) { foundUp = true; }

				if (dir != UP && !foundDown && verifyChar(row + i, col, UNKNOWN).second && verifyChar(row + i - 1, col, DESTROYED).second)
				{ foundDown = true; attackQueue.push_front(make_pair(row + i + 1, col + 1)); }
				else if (!verifyChar(row + i - 1, col, DESTROYED).second) { foundDown = true; }
			}
		}
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
	mNumOfRows = numRows;
	mNumOfCols = numCols;
	mPlayerNum = player;
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
	AbstractPlayer::init(path);
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

	random_shuffle(std::begin(valid_moves), std::end(valid_moves));
	for (auto move : valid_moves)
	{
		mMovesQueue.push(move);
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
pair<int, int> SmartPlayer::attackFromPriorityQuque(queue<pair<int, int>>& priorityQueue)
{
	auto& move = priorityQueue.front();
	while (mBoard[move.first - 1][move.second - 1] != UNKNOWN)
	{
		if(priorityQueue.size() > 0 )
		{
			move = priorityQueue.front();
			priorityQueue.pop();
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
	if (mMediumPriorityQueue.size() > 0)
	{
		auto move = attackFromPriorityQuque(mMediumPriorityQueue);
		if (isPointValid(move))
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
	auto counter = 0;
	for (auto row = 0; row < mNumOfRows; row++)
	{
		for (auto col = 0; col < mNumOfCols; col++)
		{
			auto label = mBoard[row][col];
			switch (label)
			{
			case DESTROYED:
				if(isNearChar(row, col, DESTROYED, &dir)) { addTarget(row, col, mHighPriorityQueue, dir); }
				else
				{
					if (isPointValid(row, col - 1)) { addTarget(row, col - 1, mHighPriorityQueue); }
					if (isPointValid(row - 1, col)) { addTarget(row - 1, col, mHighPriorityQueue); }
					if (isPointValid(row, col + 1)) { addTarget(row, col + 1, mHighPriorityQueue); }
					if (isPointValid(row + 1, col)) { addTarget(row + 1, col, mHighPriorityQueue); }

				}
				break;
			case UNKNOWN:
					counter = (verifyChar(row, col + 1, EMPTY).second ? 1 : 0)
						+ (verifyChar(row, col - 1, EMPTY).second ? 1 : 0)
						+ (verifyChar(row + 1, col, EMPTY).second ? 1 : 0)
						+ (verifyChar(row - 1, col, EMPTY).second ? 1 : 0);
					if (counter >= 3)
					{
						addTarget(row, col, mMediumPriorityQueue);
					}
				break;
			case SANK:
			case SHIP:
			default:
				break;
			}
		}
	}
}

void SmartPlayer::sinkShip(int srow, int scol, Direction dir)
{
	auto end = min(dir == HORIZONAL ? mNumOfCols : mNumOfRows, (dir == HORIZONAL ? scol : srow) + SHIP_MAX_LENGTH);
	auto start = max(0, (dir == HORIZONAL ? scol : srow) - SHIP_MAX_LENGTH);

	//If the ship is horizonal we keep the row constant and move along the columns.
	//If the ship is not horizonal (vertical) we keep the col constant and move along the rows.
	if(dir == HORIZONAL)
	{
		for (auto i = 0; replaceChar(srow, scol + i, DESTROYED, SANK) && i < SHIP_MAX_LENGTH; i++) {}
		for (auto i = 0; replaceChar(srow, scol - i, DESTROYED, SANK) && i < SHIP_MAX_LENGTH; i++) {}
	}
	else
	{
		for (auto i = 0; replaceChar(srow + i, scol, DESTROYED, SANK) && i < SHIP_MAX_LENGTH; i++) {}
		for (auto i = 0; replaceChar(srow - i, scol, DESTROYED, SANK) && i < SHIP_MAX_LENGTH; i++) {}
	}
}

void SmartPlayer::sinkShip(int row, int col)
{
	//pair<int, int> start_point = make_pair(mNumOfRows - 1, mNumOfCols - 1);
	if (verifyChar(row, col + 1, DESTROYED).second || verifyChar(row, col - 1, DESTROYED).second)
	{
		sinkShip(row, col, HORIZONAL);
	}
	else if (verifyChar(row + 1, col, DESTROYED).second || verifyChar(row - 1, col, DESTROYED).second)
	{
		sinkShip(row, col, VERTICAL);
	}
	else
	{
		replaceChar(row, col, DESTROYED, SANK);
	}
}

void SmartPlayer::outlineSunkenEnemyShips(int row, int col, Direction dir)
{

	int i;
	//If the ship is horizonal we keep the row constant and move along the columns.
	//If the ship is not horizonal (vertical) we keep the col constant and move along the rows.	

	// TODO - code duplication

	// The for loops take care of
	//  OOO <---these
	// OXXXO
	//  OOO <--- and these, in the horizonal case 
	//
	//  O
	// OXO
	// OXO
	// OXO
	//  O
	// ^ ^
	// these in the vertical case
	//
	// And at the end of the loop we take care of:
	//			   OOO 
	//  this ---> OXXXO <--- and this, in the horizonal case
	//			   OOO  
	//
	//  O <--- this
	// OXO
	// OXO
	// OXO
	//  O <--- and this, in the vertical case 
	if (dir == HORIZONAL)
	{
		for (i = 0; verifyChar(row, col + i, SANK).second && i < SHIP_MAX_LENGTH; i++)
		{
			replaceChar(row + 1, col + i, UNKNOWN, EMPTY);
			replaceChar(row - 1, col + i, UNKNOWN, EMPTY);
		}
		replaceChar(row, col + i, UNKNOWN, EMPTY);
		for (i = 0; verifyChar(row, col - i, SANK).second && i < SHIP_MAX_LENGTH; i++)
		{
			replaceChar(row + 1, col - i, UNKNOWN, EMPTY);
			replaceChar(row - 1, col - i, UNKNOWN, EMPTY);
		}
		replaceChar(row, col - i, UNKNOWN, EMPTY);
	}
	else
	{
		for (i = 0; verifyChar(row + i, col, SANK).second && i < SHIP_MAX_LENGTH; i++)
		{
			replaceChar(row + i, col + 1, UNKNOWN, EMPTY);
			replaceChar(row + i, col - 1, UNKNOWN, EMPTY);
		}
		replaceChar(row + i, col, UNKNOWN, EMPTY);
		for (i = 0; verifyChar(row - i, col, SANK).second && i < SHIP_MAX_LENGTH; i++)
		{
			replaceChar(row - i, col + 1, UNKNOWN, EMPTY);
			replaceChar(row - i, col - 1, UNKNOWN, EMPTY);
		}
		replaceChar(row - i, col, UNKNOWN, EMPTY);
	}
}

// TODO - outlineSunkenEnemyShips and sinkShip have some code duplicateion
void SmartPlayer::outlineSunkenEnemyShips(int row, int col)
{
	if(verifyChar(row, col + 1, SANK).second || verifyChar(row, col - 1, SANK).second)
	{
		outlineSunkenEnemyShips(row, col, HORIZONAL);
	}
	else if (verifyChar(row + 1, col, SANK).second || verifyChar(row - 1, col, SANK).second)
	{
		outlineSunkenEnemyShips(row, col, VERTICAL);
	}
	else
	{
		replaceChar(row + 1, col, UNKNOWN, EMPTY);
		replaceChar(row - 1, col, UNKNOWN, EMPTY);
		replaceChar(row, col + 1, UNKNOWN, EMPTY);
		replaceChar(row, col - 1, UNKNOWN, EMPTY);
	}


}

// TODO - add non-file player enemy ships outlining
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
		if (result == AttackResult::Hit) { analyzeAttackResult(); }
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