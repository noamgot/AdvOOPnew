#include "SmartPlayer.h"
#include "GameUtilities.h"



bool SmartPlayer::isShip(int row, int col)
{
	if (isPointValid(row, col))
	{
		if (row == 1 && col > 5)
		{
			printf("%c", mBoard[row][col]);
		}
		return verifyChar(row, col, (mPlayerNum == 1) ? tolower(DESTROYER) : DESTROYER)
			|| verifyChar(row, col, (mPlayerNum == 1) ? tolower(BOAT) : BOAT)
			|| verifyChar(row, col, (mPlayerNum == 1) ? tolower(MISSLE_SHIP) : MISSLE_SHIP)
			|| verifyChar(row, col, (mPlayerNum == 1) ? tolower(SUBMARINE) : SUBMARINE)
			|| verifyChar(row, col, DESTROYED);
	}
	return false;
}

bool SmartPlayer::isShipOutline(int row, int col)
{
	return isShip(row + 1, col) || isShip(row - 1, col) || isShip(row, col + 1) || isShip(row, col - 1);
}

bool SmartPlayer::isPointValid(int row, int col)
{
	return GameUtilities::isLegalMove(row + 1, col + 1, mNumOfRows, mNumOfCols);
}

void SmartPlayer::putChar(int row, int col, char new_char)
{
		mBoard[row][col] = new_char;
}

bool SmartPlayer::replaceChar(int row, int col, char old_char, char new_char)
{
	if (verifyChar(row, col, old_char))
	{
		putChar(row, col, new_char);
		return true;
	}
	return false;
}

bool SmartPlayer::addTarget(int row, int col)
{
	if (mBoard[row][col] == WATER)
	{
		mSmartMoveQueue.push_front(make_pair(row + 1, col + 1));
		return true;
	}
	return false;
}

bool SmartPlayer::verifyChar(int row, int col, char c)
{
	return mBoard[row][col] == c;
}

void SmartPlayer::setBoard(int player, const char** board, int numRows, int numCols)
{
	mNumOfRows = numRows;
	mNumOfCols = numCols;
	mPlayerNum = player;
	auto spaceIOccupy = 0;
	AbstractPlayer::setBoard(player, board, numRows, numCols);

	for(auto row = 0; row < mNumOfRows; row++)
	{
		for(auto col = 0; col < mNumOfCols; col++)
		{
			if (isShipOutline(row, col))
			{
				replaceChar(row, col, WATER, EMPTY);
				
			}
			else if (isShip(row, col))
			{
				mMyCoords.insert(make_pair(row, col));
			}
		}
	}
}

bool SmartPlayer::init(const string& path)
{
	AbstractPlayer::init(path);
	vector<pair<int, int>> valid_moves;
	mIsFilePlayer = false;
	for (auto row = 0; row < mNumOfRows; row++)
	{
		for (auto col = 0; col < mNumOfCols; col++)
		{
			if (mBoard[row][col] != WATER || mBoard[row][col] != EMPTY)
			{
				valid_moves.push_back(make_pair(row + 1,col + 1));
			}
		}
	}
	random_shuffle(valid_moves.begin(), valid_moves.end());
	for (auto move : valid_moves)
	{
		mSmartMoveQueue.push_back(move);
	}
	return true;
}

pair<int, int> SmartPlayer::attack()
{
	auto& move = make_pair(-1,-1);
	if (mSmartMoveQueue.size()== 0)
	{
		return move;
	}
	do
	{
		move = mSmartMoveQueue.front();
		mSmartMoveQueue.pop_front();
	} while (mBoard[move.first - 1][move.second - 1] != WATER && mSmartMoveQueue.size() > 1);
	
	return move;
}

void SmartPlayer::analyzeEnemy(pair<int,int> hitPoint, AttackResult result)
{
	//Only the file player can hit himselft or the same spot twice!
	//Once we know it is the file player we're up against we won't take into account his misses!
	if (mEnemyAttackCoords.find(hitPoint) != mEnemyAttackCoords.end() || (result != AttackResult::Miss && mMyCoords.find(hitPoint) == mMyCoords.end()))
	{
		//TODO - should disregard misses (except of marking on board)
		mIsFilePlayer = true;
	}
	mEnemyAttackCoords.insert(make_pair(hitPoint.first, hitPoint.second));
}

Direction SmartPlayer::scanTheVicinity(int row, int col)
{

	if ((isPointValid(row, col + 1) && verifyChar(row, col + 1, DESTROYED)) || (isPointValid(row, col + 1) && verifyChar(row, col - 1, DESTROYED)))
	{
		return Direction::HORIZONAL;
	}
	if ((isPointValid(row + 1, col) && verifyChar(row + 1, col, DESTROYED)) || (isPointValid(row - 1, col) && verifyChar(row - 1, col, DESTROYED)))
	{
		return Direction::VERTICAL;
	}
	return Direction::NONE;
	//TODO - add deeper scanning for eSign::EMPTY and edge coordinates
}

void SmartPlayer::outlineSunkenEnemyShip(int row, int col, int row_mod, int col_mod)
{
	int counter;
	if(row_mod == 0 && col_mod == 0)
	{
		outlineSunkenEnemyShip(row + 1, col, 1, 0);
		outlineSunkenEnemyShip(row - 1, col, -1, 0);
		outlineSunkenEnemyShip(row, col + 1, 0, 1);
		outlineSunkenEnemyShip(row, col - 1, 0, -1);
	}
	else if (!isPointValid(row, col))
	{
		return;
	}
	else
	{
		counter = 0;
		if (isShipOutline(row + 1, col))
		{
			replaceChar(row, col, WATER, EMPTY);
			counter++;
		}
		if (isShipOutline(row - 1, col))
		{
			replaceChar(row, col, WATER, EMPTY);
			counter++;
		}
		if (isShipOutline(row, col + 1))
		{
			replaceChar(row, col, WATER, EMPTY);
			counter++;
		}
		if (isShipOutline(row, col - 1))
		{
			replaceChar(row, col, WATER, EMPTY);
			counter++;
		}
		if (counter < 3)
		{
			outlineSunkenEnemyShip(row + row_mod, col + col_mod, row_mod, col_mod);
		}
	}
	
}

//TODO - add non-file player enemy ships outlining
void SmartPlayer::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (!GameUtilities::isLegalMove(row, col, mNumOfRows, mNumOfCols)) // ignore invalid moves
	{
		return;
	}
	int myRow = row - 1, myCol = col - 1;

	if (player != mPlayerNum)
	{
		analyzeEnemy(make_pair(myRow, myCol), result);
	}

	if (result != AttackResult::Miss && mBoard[myRow][myCol] == WATER)
	{
		putChar(myRow, myCol, DESTROYED); //destroyed something
		switch (result)
		{
		case AttackResult::Hit :
			if (isPointValid(myRow - 1, myCol))
			{
				addTarget(myRow - 1, myCol);
			}
			if (isPointValid(myRow + 1, myCol))
			{
				addTarget(myRow + 1, myCol);

			}
			if (isPointValid(myRow, myCol - 1))
			{
				addTarget(myRow, myCol - 1);
			}
			if (isPointValid(myRow, myCol + 1))
			{
				addTarget(myRow, myCol + 1);
			}
			break;
		case AttackResult::Sink:
			outlineSunkenEnemyShip(myRow, myCol);
			break;

		default:
			break;
		}
		
	}
	else
	{
		mBoard[myRow][myCol] = EMPTY; // empty
	}
	/*FILE *f = fopen("SmartBoard.txt", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
	}
	for (auto i = 0; i < 10; i++)
	{
		for (auto j = 0; j < 10; j++)
		{
			fprintf(f, "%c", mBoard[i][j]);
		}
		fprintf(f, "\n");
	}
	fclose(f);*/
	
}

IBattleshipGameAlgo* GetAlgorithm()
{
	IBattleshipGameAlgo *newP = new SmartPlayer;
	return newP;
}


