#pragma once
#include <set>
#include <deque>
#include "CommonUtilities.h"
#include "MyBoardData.h"
#include <list>
#include <ctime>
#include <algorithm>

enum Direction
{
	UP,
	DOWN,
	RIGHT,
	LEFT,
	BOTTOM,
	TOP,
	VERTICAL,
	HORIZONAL,
	DEPTH,
	NONE
};

enum eSign : char
{
	EMPTY = 'O',
	SHIP = 'S',
	DESTROYED = 'D',
	SANK = 'X',
	UNKNOWN = ' '
};

class SmartPlayer : IBattleshipGameAlgo
{
protected:
	static const int NUMBER_OF_DIRECTIONS = 6;
	static const int SHIP_MAX_LENGTH = 4;

	// A set for the coordinates of the player's ships
	std::set<Coordinate>  mMyCoords;			

	// A set for keeping track of where enemy attacks land.
	std::set<Coordinate>  mEnemyAttackCoords;

	// A queue for high priority (follow up) attacks
	std::deque<Coordinate> mHighPriorityQueue;

	int mRows;
	int mCols;
	int mDepth;
	int mPlayerNum;
	int mShipsCount; // number of living ships - starting from DEFAULT_SHIPS_COUNT
	MyBoardData mBoard;
	std::deque<Coordinate> mMovesQueue;

	//Returns whether the coordinates are valid - in the inner representation (0 - COL/ROW SIZE -1)
	bool SmartPlayer::isPointValid(int row, int col, int depth) const
	{
		return CommonUtilities::isLegalMove(row + 1, col + 1, depth + 1, mRows, mCols, mDepth);
	}
	//Returns whether the coordinates are valid - in the inner representation (0 - COL/ROW SIZE -1) (overload)
	bool SmartPlayer::isPointValid(Coordinate point) const
	{
		return CommonUtilities::isLegalMove(point, mRows, mCols, mDepth);
	}

	//Replaces the old_char with new_char and we return true. if the square did not hold old_char nothing happens and we return false.
	//If reverse = true the char will be replaced only if it is NOT equal to old_char.
	bool replaceChar(int row, int col, int depth, char old_char, char new_char, bool reverse = false);

	// Adds the coordinates to the attack queue if they are unknown or searches for the first unknown coordinates in the direction dir points.
	void addTarget(int row, int col, int depth, std::deque<Coordinate>& attackQueue, Direction dir = NONE);

	// The main loop of addTarget (born from code duplication).
	void addTargetLoop(int row, int col, int depth, int rowMod, int colMod, int depthMod, std::deque<Coordinate>& attackQueue, Direction dir, Direction orientation, Direction positiveDirection, Direction negativeDirection);

	//Returns a pair of booleans, the first is true iff the point is a valid point and the second is true iff these coordinates contain the char c.
	std::pair<bool, bool> verifyChar(int row, int col, int depth, char c);

	//Returns whether this square is adjacent to a ship. Does not assumes coordinates are valid.
	bool isNearChar(int row, int col, int depth, eSign s, Direction* dir = nullptr); 

	Coordinate attackFromPriorityQuque(std::deque<Coordinate>& priorityQueue);

	// Convert the board the player received so that it is labeled by the enum eSign.
	// Used for analysis purposes
	void reLabelTheBoard();

	// Adds new targets to high and medium priority queues based on the results of the most recent attack.
	void analyzeAttackResult();

	// Wrapper fucntion for outlineSunkenEnemyShips(int row, int col) for getting the direction first.
	void outlineSunkenEnemyShips(int row, int col, int depth);				    

	// Mark the area around a sunken ship as eSight::Empty so it won't be targeted.
	// In this function we take care of:
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
	//			   OOO 
	//  this ---> OXXXO <--- and this, in the horizonal case
	//			   OOO  
	//
	//  O <--- this
	// OXO
	// OXO
	// OXO
	//  O <--- and this, in the vertical case 
	void outlineSunkenEnemyShips(int row, int col, int depth, Direction dir); 

	// Wrapper fucntion for void sinkShip(int srow, int scol, Direction dir) for getting the direction first.
	void sinkShip(int row, int col, int depth);

	// Transforms a destroyed ship to a sunken one (so that its outline can be labeled empty.
	void sinkShip(int row, int col, int depth, Direction dir);

	// The main loop of outlineSunkenEnemyShips (born from code duplication).
	void SmartPlayer::outlineLoop(int row, int col, int depth, int rowMod, int colMod, int depthMod, bool reverse);

	// The function pokes the coordinates adjacent to the (row,col) it is given and depening
	// On the value of outline decides it to call outlineSunkenEnemyShips(row, col , dir)  or sinkShip(row, col ,dir)
	// where dir = HORIZONAL or VERTICAL depending on the results of the poking 
	bool findDirection(int row, int col, int depth, bool outline);

	bool init();
	

public:
	SmartPlayer() : mRows(-1), mCols(-1), mDepth(-1), mPlayerNum(-1), mShipsCount(-1) {}
	~SmartPlayer(){}
	void setBoard(const BoardData& board) override; // called once to notify player about his board // called once to notify player on his boardd
	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override; // notify on last move result
	void setPlayer(int player) override;
	Coordinate attack() override; // ask player for his move
	static IBattleshipGameAlgo* GetAlgorithm();
};