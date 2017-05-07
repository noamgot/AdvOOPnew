#include "SmartPlayer.h"
#include "GameUtilities.h"


SmartPlayer::~SmartPlayer()
{
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
			if (mBoard[row][col] != eShipChar::WATER)
			{
				mMyCoords.insert(make_pair(row, col));
				doIfValid(row + 1, col, false, false, true, eSign::EMPTY, eShipChar::WATER);
				doIfValid(row - 1, col, false, false, true, eSign::EMPTY, eShipChar::WATER);
				doIfValid(row, col + 1, false, false, true, eSign::EMPTY, eShipChar::WATER);
				doIfValid(row, col + 1, false, false, true, eSign::EMPTY, eShipChar::WATER);
			}
		}
	}
}

pair<bool, bool> SmartPlayer::doIfValid(int row, int col, bool hit_op, bool target_op, bool swap_op, char new_char, char old_char)
{
	if (GameUtilities::isLegalMove(row + 1, col + 1, mNumOfRows, mNumOfCols))
	{
		if (swap_op && mBoard[row][col] == old_char)
		{
			mBoard[row][col] = new_char;
		}
		else if (hit_op)
		{
			mBoard[row][col] = eSign::DESTROYED;
		}
		else if (target_op)
		{
			if (mBoard[row][col] == eShipChar::WATER)
			{
				mSmartMoveQueue.push_front(make_pair(row, col));
			}
		}
		return make_pair(true,(!target_op && !hit_op && !swap_op) || (swap_op ^ swap_op ^ hit_op));
	}
	return make_pair(false,false);
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
			if (mBoard[row][col] != eShipChar::WATER || mBoard[row][col] != eSign::EMPTY)
			{
				valid_moves.push_back(make_pair(row,col));
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
	} while (mBoard[move.first][move.second] != eShipChar::WATER && mSmartMoveQueue.size() > 1);
	
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

	if ((doIfValid(row, col + 1).first && mBoard[row][col + 1] == eSign::DESTROYED) || (doIfValid(row, col - 1).first && mBoard[row][col - 1] == eSign::DESTROYED))
	{
		return Direction::HORIZONAL;
	}
	if ((doIfValid(row + 1, col).first && mBoard[row + 1][col] == eSign::DESTROYED) || (doIfValid(row - 1, col).first && mBoard[row - 1][col] == eSign::DESTROYED))
	{
		return Direction::VERTICAL;
	}
	//TODO - add deeper scanning for eSign::EMPTY and edge coordinates
}

void SmartPlayer::outlineSunkenEnemyShip(int row, int col)
{
	pair<pair<bool, bool>, Direction> dir[NUMBER_OF_DIRECTIONS];
	int row_mod, col_mod;
	dir[0].first = doIfValid(row + 1, col, false, false, true, eShipChar::WATER, eSign::EMPTY); //up
	dir[0].second = Direction::UP;
	dir[1].first = doIfValid(row - 1, col, false, false, true, eShipChar::WATER, eSign::EMPTY); //down
	dir[1].second = Direction::DOWN;
	dir[2].first = doIfValid(row, col + 1, false, false, true, eShipChar::WATER, eSign::EMPTY); //right 
	dir[2].second = Direction::RIGHT;
	dir[3].first = doIfValid(row, col - 1, false, false, true, eShipChar::WATER, eSign::EMPTY); //left
	dir[3].second = Direction::LEFT;
	
	for (auto i = 0; i < NUMBER_OF_DIRECTIONS; ++i)
	{
		if (dir[i].first.first && !dir[i].first.second)
		{
			switch (dir[i].second)
			{
			case Direction::UP:
				row_mod = 1;
				col_mod = 0;
				break;
			case Direction::DOWN:
				row_mod = -1;
				col_mod = 0;
				break;
			case Direction::RIGHT:
				row_mod = 0;
				col_mod = 1;
				break;
			case Direction::LEFT:
				row_mod = 0;
				col_mod = -1;
				break;
			default:
				break;
			}
			auto j = 1;
			do 
			{
				if (dir[i].second == Direction::LEFT || dir[i].second == Direction::RIGHT)
				{
					doIfValid(row + j * row_mod - 1, col + j * col_mod, false, false, true, eShipChar::WATER, eSign::EMPTY);
					doIfValid(row + j * row_mod + 1, col + j * col_mod, false, false, true, eShipChar::WATER, eSign::EMPTY);
				}
				else if (dir[i].second == Direction::UP || dir[i].second == Direction::DOWN)
				{
					doIfValid(row + j * row_mod, col + j * col_mod + 1, false, false, true, eShipChar::WATER, eSign::EMPTY);
					doIfValid(row + j * row_mod, col + j * col_mod - 1, false, false, true, eShipChar::WATER, eSign::EMPTY);
				}

				j++; 
			} while (mBoard[row + j * row_mod][col + j * col_mod] == eSign::DESTROYED);
			mBoard[row + j * row_mod][col + j * col_mod] = eSign::EMPTY;
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

	if (result != AttackResult::Miss && mBoard[myRow][myCol] == eShipChar::WATER)
	{
		doIfValid(myRow, myCol, true); //destroyed something
		switch (result)
		{
		case AttackResult::Hit :
			mDir = scanTheVicinity(myRow, myCol);
			doIfValid(row, col, true);
			if (mDir == Direction::NONE || mDir == Direction::HORIZONAL)
			{

					doIfValid(row, col + 1, false, true);
					doIfValid(row, col - 1, false, true);
				
			}
			if (mDir == Direction::NONE || mDir == Direction::VERTICAL)
			{
					doIfValid(row + 1, col, false, true);
					doIfValid(row - 1, col, false, true);
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
		mBoard[myRow][myCol] = eSign::EMPTY; // empty
	}
	
}

IBattleshipGameAlgo* GetAlgorithm()
{
	IBattleshipGameAlgo *newP = new SmartPlayer;
	return newP;
}


