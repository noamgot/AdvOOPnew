#include "Player.h"

using namespace std;


bool Player::isNearChar(int row, int col, int depth, eSign s, Direction* dir)
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

bool Player::replaceChar(int row, int col, int depth, char old_char, char new_char, bool reverse)
{
	auto pr = verifyChar(row, col, depth,  old_char);
	if (pr.second != reverse)
	{
		_Board.setChar(Coordinate(row, col, depth), new_char);
		return true;
	}
	return false;
}

void Player::addTargetLoop(int row, int col, int depth, int rowMod, int colMod, int depthMod, deque<Coordinate>& attackQueue, Direction dir, Direction orientation, Direction positiveDirection, Direction negativeDirection)
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

void Player::addTarget(int row, int col, int depth, deque<Coordinate>& attackQueue, Direction dir)
{
	//auto maxDist = 4;
	//auto foundUp = false, foundDown = false, foundLeft = false, foundRight = false;
	// This means we are just adding a regular target (have no additional info)
	if (dir == NONE && verifyChar(row, col, depth, UNKNOWN).second) { attackQueue.push_front(Coordinate(row, col, depth)); }

	else // In this case we have some additional information
	{
		addTargetLoop(row, col, depth, 0, 1, 0, attackQueue, dir, HORIZONAL, RIGHT, LEFT);
		addTargetLoop(row, col, depth, 1, 0, 0, attackQueue, dir, VERTICAL, DOWN, UP);
		addTargetLoop(row, col, depth, 0, 0, 1, attackQueue, dir, DEPTH, BOTTOM, TOP);
	}
}

pair<bool, bool> Player::verifyChar(int row, int col, int depth, char c)
{
	Coordinate cord(row, col, depth);
	if (isPointValid(cord))
	{
		return make_pair(true, _Board.charAt(cord) == c);
	}
	return make_pair(false, false);
}

void Player::reLabelTheBoard()
{
	for (auto row = 1; row <= _Rows; row++)
	{
		for (auto col = 1; col <= _Cols; col++)
		{
			for (auto depth = 1; depth <= _Depth; depth++)
			{
				Coordinate cord(row, col, depth);
				if (_Board.charAt(cord) != CommonUtilities::eShipChar::WATER)
				{
					_Board.setChar(cord, SHIP);
					_MyCoords.insert(cord);
				}
				else
				{
					_Board.setChar(cord, UNKNOWN);
				}
			}
		}
	}
}

void Player::setBoard(const BoardData &board)
{
	_Rows = board.rows();
	_Cols = board.cols();
	_Depth = board.depth();
	_Board = MyBoardData(_Rows, _Cols, _Depth);
	// Copy the board
	for (auto row = 1; row <= _Rows; row++)
	{
		for (auto col = 1; col <= _Cols; col++)
		{
			for (auto depth = 1; depth <= _Depth; depth++)
			{
				auto coord = Coordinate(row, col, depth);
				_Board.setChar(coord, board.charAt(coord));
			}
		}
	}
	
	reLabelTheBoard();

	for(auto row = 1; row <= _Rows; ++row)
	{
		for(auto col = 1; col <= _Cols; ++col)
		{
			for (auto depth = 1; depth <= _Depth; depth++)
			{
				if (isNearChar(row, col, depth, SHIP))
				{
					
					replaceChar(row, col, depth, UNKNOWN, EMPTY);
				}
			}
		}
	}
	queueInit();
}

bool Player::queueInit()
{
	vector<Coordinate> valid_moves;
	for (auto row = 1; row <= _Rows; row++)
	{
		for (auto col = 1; col <= _Cols; col++)
		{
			for (auto depth = 1; depth <= _Depth; depth++)
			{
				Coordinate cord(row, col, depth);
				if (_Board.charAt(cord) == UNKNOWN)
				{
					valid_moves.push_back(Coordinate(row, col, depth));
				}
			}
		}
	}
	srand(static_cast<unsigned int>(time(nullptr))); // Seed the PRG for all of our random needs.

	random_shuffle(begin(valid_moves), end(valid_moves));
	for (auto move : valid_moves)
	{
		_MovesQueue.push_back(move);
	}
	return true;
}

Coordinate Player::attackFromPriorityQuque(deque<Coordinate>& priorityQueue)
{
	auto& move = priorityQueue.front();
	while (_Board.charAt(move) != UNKNOWN)
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

Coordinate Player::attack()
{
	if (_HighPriorityQueue.size() > 0)
	{
		auto move = attackFromPriorityQuque(_HighPriorityQueue);
		if(isPointValid(move))
		{
			return move;
		}
	}
	if (_MovesQueue.size() > 0)
	{
		auto move = attackFromPriorityQuque(_MovesQueue);
		if (isPointValid(move))
		{
			return move;
		}
	}
	return Coordinate(-1, -1, -1);
}

void Player::analyzeAttackResult()
{
	auto dir = NONE;
	for (auto row = 1; row <= _Rows; row++)
	{
		for (auto col = 1; col <= _Cols; col++)
		{
			for (auto depth = 1; depth <= _Depth; depth++)
			{
				Coordinate cord(row, col, depth);
				if (_Board.charAt(cord) == DESTROYED)
				{
					if (isNearChar(row, col, depth, DESTROYED, &dir)) { addTarget(row, col, depth, _HighPriorityQueue, dir); }
					else
					{
						if (isPointValid(row, col - 1, depth)) { addTarget(row, col - 1, depth, _HighPriorityQueue); }
						if (isPointValid(row - 1, col, depth)) { addTarget(row - 1, col, depth, _HighPriorityQueue); }
						if (isPointValid(row, col + 1, depth)) { addTarget(row, col + 1, depth, _HighPriorityQueue); }
						if (isPointValid(row + 1, col, depth)) { addTarget(row + 1, col, depth, _HighPriorityQueue); }
						if (isPointValid(row, col, depth + 1)) { addTarget(row, col, depth + 1, _HighPriorityQueue); }
						if (isPointValid(row, col, depth - 1)) { addTarget(row, col, depth - 1, _HighPriorityQueue); }
					}
				}
			}
		}
	}
}

void Player::sinkShip(int row, int col, int depth, Direction dir)
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

void Player::sinkShip(int row, int col, int depth)
{
	if (!findDirection(row, col, depth, false))
	{
		replaceChar(row, col, depth, DESTROYED, SANK);
	}
}

void Player::outlineLoop(int row, int col, int depth, int rowMod, int colMod, int depthMod, bool reverse)
{
	auto i = 0;
	for (; verifyChar(row + (reverse ? -1 : 1) * i * rowMod, col + (reverse ? -1 : 1) * i * colMod, depth + (reverse ? -1 : 1) * i * depthMod, SANK).second && i < SHIP_MAX_LENGTH; i++)
	{
		replaceChar(row + (reverse ? -1 : 1) * i * rowMod + 1 * colMod * depthMod, col + (reverse ? -1 : 1) * i * colMod + 1 * rowMod * depthMod, depth + (reverse ? -1 : 1) * i * depthMod + 1 * colMod * rowMod, UNKNOWN, EMPTY);
		replaceChar(row + (reverse ? -1 : 1) * i * rowMod - 1 * colMod * depthMod, col + (reverse ? -1 : 1) * i * colMod - 1 * rowMod * depthMod, depth + (reverse ? -1 : 1) * i * depthMod - 1 * colMod * rowMod, UNKNOWN, EMPTY);
	}
	replaceChar(row + (reverse ? -1 : 1) * i * rowMod, col + (reverse ? -1 : 1) * i * colMod, depth + (reverse ? -1 : 1) * i * depthMod, UNKNOWN, EMPTY);
}

bool Player::findDirection(int row, int col, int depth, bool outline)
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

void Player::outlineSunkenEnemyShips(int row, int col, int depth, Direction dir)
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

void Player::outlineSunkenEnemyShips(int row, int col, int depth)
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

void Player::notifyOnAttackResult(int player, Coordinate move, AttackResult result)
{
	if (!CommonUtilities::isLegalMove(move, _Rows, _Cols, _Depth)) // ignore invalid moves
	{
		return;
	}
	if (result != AttackResult::Miss && verifyChar(move.row, move.col, move.depth, UNKNOWN).second)
	{
		replaceChar(move.row, move.col, move.depth, UNKNOWN, DESTROYED);
		if (result == AttackResult::Hit)
		{
			analyzeAttackResult();
		}
		else 
		{
			sinkShip(move.row, move.col, move.depth);
			outlineSunkenEnemyShips(move.row, move.col, move.depth);
		}
	}
	else
	{
		replaceChar(move.row, move.col, move.depth, UNKNOWN, EMPTY);
	}	
}

void Player::setPlayer(int player)
{
	_PlayerNum = player;
}

IBattleshipGameAlgo* GetAlgorithm()
{
	return new Player;
}
