#include "SmartPlayer.h"
#include "GameUtilities.h"



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

void SmartPlayer::addTarget(int row, int col, queue<pair<int, int>> attackQueue, Direction dir)
{
	auto maxDist = 4;
	auto foundUp = false, foundDown = false, foundLeft = false, foundRight = false;
	// This means we are just adding a regular target (have no additional info)
	if (dir == NONE && verifyChar(row, col, UNKNOWN).second) { attackQueue.push(make_pair(row + 1, col + 1)); }

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
				{ foundRight = true; attackQueue.push(make_pair(row + 1, col + 1 + i)); }
				else if (!verifyChar(row, col + i - 1, DESTROYED).second) { foundRight = true; }

				if (dir != RIGHT && !foundLeft && verifyChar(row, col - i, UNKNOWN).second && verifyChar(row, col - i + 1, DESTROYED).second)
				{ foundLeft = true;attackQueue.push(make_pair(row + 1, col + 1 - i)); }
				else if (!verifyChar(row, col - i + 1, DESTROYED).second) { foundLeft = true; }
			}
		}

		if (dir == VERTICAL || dir == UP || dir == DOWN)
		{
			for (auto i = 1; i < maxDist; i++)
			{
				if (dir != DOWN && !foundUp && verifyChar(row + i, col, UNKNOWN).second && verifyChar(row + i - 1, col, DESTROYED).second)
				{ foundUp = true; attackQueue.push(make_pair(row + 1 + i, col + 1)); }
				else if (!verifyChar(row + i - 1, col, DESTROYED).second) { foundUp = true; }

				if (dir != UP && !foundDown && verifyChar(row - 1, col, UNKNOWN).second && verifyChar(row - i + 1, col, DESTROYED).second)
				{ foundDown = true; attackQueue.push(make_pair(row + 1 - i, col + 1)); }
				else if (!verifyChar(row - i + 1, col, DESTROYED).second) { foundDown = true; }
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
			if (mBoard[row][col] != WATER)
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

pair<int, int> SmartPlayer::attackFromPriorityQuque(queue<pair<int,int>> priorityQueue)
{
	auto& move = priorityQueue.front();
	while (mBoard[move.first - 1][move.second - 1] != UNKNOWN && priorityQueue.size() > 1)
	{
		move = priorityQueue.front();
		priorityQueue.pop();
	}
	return move;	
}

pair<int, int> SmartPlayer::attack()
{
	if (mHighPriorityQueue.size() > 0)
	{
		return attackFromPriorityQuque(mHighPriorityQueue);
	}
	if (mMediumPriorityQueue.size() > 0)
	{
		return attackFromPriorityQuque(mMediumPriorityQueue);
	}
	if (mMovesQueue.size() > 0)
	{
		return attackFromPriorityQuque(mMovesQueue);
	}
	return make_pair(-1,-1);
}

void SmartPlayer::analyzeAttackResult()
{
	auto maxDist = 5;
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
					if (isPointValid(row - 1, col)) { addTarget(row - 1, col, mHighPriorityQueue); }
					if (isPointValid(row + 1, col)) { addTarget(row + 1, col, mHighPriorityQueue); }
					if (isPointValid(row, col - 1)) { addTarget(row, col - 1, mHighPriorityQueue); }
					if (isPointValid(row, col + 1)) { addTarget(row, col + 1, mHighPriorityQueue); }

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
			case SUNK:
			case SHIP:
			default:
				break;
			}
		}
	}
}

// TODO - perfect this
void SmartPlayer::analyzeEnemy(pair<int,int> hitPoint, AttackResult result)
{
	// Only the file player can hit himselft or the same spot twice!
	// Once we know it is the file player we're up against we won't take into account his misses!
	if (mEnemyAttackCoords.find(hitPoint) != mEnemyAttackCoords.end() || (result != AttackResult::Miss && mMyCoords.find(hitPoint) == mMyCoords.end()))
	{
		// TODO - should disregard misses (except of marking on board)
		//mIsFilePlayer = true;
	}
	mEnemyAttackCoords.insert(hitPoint);
}

//Direction SmartPlayer::scanTheVicinity(int row, int col)
//{
//	// TODO - has some code duplication, maybe we can make a function out of it?
//	auto pr1 = verifyChar(row, col + 1, DESTROYED);
//	auto pr2 = verifyChar(row, col - 1, DESTROYED);
//	if((pr1.first && pr2.second) || (pr2.first && pr2.second))
//	{
//		return HORIZONAL;
//	}
//	pr1 = verifyChar(row + 1, col, DESTROYED);
//	pr2 = verifyChar(row - 1, col, DESTROYED);
//	if ((pr1.first && pr2.second) || (pr2.first && pr2.second))
//	{
//		return VERTICAL;
//	}
//	return NONE;
//	// TODO - add deeper scanning for EMPTY and edge coordinates
//}

void SmartPlayer::outlineEnemyShips(int row, int col)
{
	auto maxDist = 4;
	for (auto i = row; i < min(mNumOfRows, row + maxDist); i++)
	{
		for (auto j = col; j <  min(mNumOfCols, col + maxDist); j++)
		{
			if(replaceChar(i, j, DESTROYED, SUNK))
			{
				replaceChar(i, j + 1, UNKNOWN, EMPTY);
				replaceChar(i, j - 1, UNKNOWN, EMPTY);
				replaceChar(i + 1, j, UNKNOWN, EMPTY);
				replaceChar(i - 1, j, UNKNOWN, EMPTY);
			}
		}
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

	if (player != mPlayerNum)
	{
		analyzeEnemy(make_pair(myRow, myCol), result);
	}

	if (result != AttackResult::Miss && verifyChar(myRow, myCol, UNKNOWN).second)
	{
		mBoard[myRow][myCol] = DESTROYED; // destroyed something
		switch (result)
		{
		case AttackResult::Hit:
			replaceChar(myRow, myCol, UNKNOWN, DESTROYED);
			analyzeAttackResult();
			break;
		case AttackResult::Sink:
			replaceChar(myRow, myCol, UNKNOWN, SUNK);
			outlineEnemyShips(myRow, myCol);
			break;

		default:
			break;
		}
	}
	else
	{
		replaceChar(myRow, myCol, UNKNOWN, EMPTY);
	}
	//debugBoard();
}

IBattleshipGameAlgo* GetAlgorithm()
{
	IBattleshipGameAlgo *newP = new SmartPlayer;
	return newP;
}

//void SmartPlayer::debugBoard()
//{
//	auto *f = fopen("SmartBoard.txt", "w");
//	if (f == nullptr)
//	{
//		printf("Error opening file!\n");
//	}
//	for (auto i = 0; i < 10; i++)
//	{
//		for (auto j = 0; j < 10; j++)
//		{
//			fprintf(f, "%c", mBoard[i][j]);
//		}
//		fprintf(f, "\n");
//	}
//	fclose(f);
//}

