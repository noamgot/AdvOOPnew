#pragma once
#include "AbstractPlayer.h"
#include <set>
#include <deque>


enum Direction
{
	UP,
	DOWN,
	RIGHT,
	LEFT,
	VERTICAL,
	HORIZONAL,
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

class SmartPlayer :	public AbstractPlayer
{
protected:
	static const int NUMBER_OF_DIRECTIONS = 4;
	static const int SHIP_MAX_LENGTH = 4;

	// A set for the coordinates of the player's ships
	set<pair<int, int>>  mMyCoords;			

	// A set for keeping track of where enemy attacks land.
	set<pair<int, int>>  mEnemyAttackCoords;	

	// A queue for high priority (follow up) attacks
	deque<pair<int, int>> mHighPriorityQueue;

	//Returns whether the coordinates are valid - in the inner representation (0 - COL/ROW SIZE -1)
	bool SmartPlayer::isPointValid(int row, int col) const
	{
		return GameUtilities::isLegalMove(row + 1, col + 1, mNumOfRows, mNumOfCols);
	}
	//Returns whether the coordinates are valid - in the inner representation (0 - COL/ROW SIZE -1) (overload)
	bool SmartPlayer::isPointValid(pair<int, int> point) const
	{
		return GameUtilities::isLegalMove(point, mNumOfRows, mNumOfCols);
	}

	//Replaces the old_char with new_char and we return true. if the square did not hold old_char nothing happens and we return false.
	//If reverse = true the char will be replaced only if it is NOT equal to old_char.
	bool replaceChar(int row, int col, char old_char, char new_char, bool reverse = false);

	// Adds the coordinates to the attack queue if they are unknown or searches for the first unknown coordinates in the direction dir points.
	void addTarget(int row, int col, deque<pair<int, int>>& attackQueue, Direction dir = NONE);

	// The main loop of addTarget (born from code duplication).
	void addTargetLoop(int row, int col, int rowMod, int colMod, deque<pair<int, int>>& attackQueue, Direction dir, Direction orientation, Direction positiveDirection, Direction negativeDirection);

	//Returns a pair of booleans, the first is true iff the point is a valid point and the second is true iff these coordinates contain the char c.
	pair<bool, bool> verifyChar(int row, int col, char c);

	//Returns whether this square is adjacent to a ship. Does not assumes coordinates are valid.
	bool isNearChar(int row, int col, eSign s, Direction* dir = nullptr); 

	pair<int, int> attackFromPriorityQuque(deque<pair<int, int>>& priorityQueue);

	// Convert the board the player received so that it is labeled by the enum eSign.
	// Used for analysis purposes
	void reLabelTheBoard();

	// Adds new targets to high and medium priority queues based on the results of the most recent attack.
	void analyzeAttackResult();

	// Wrapper fucntion for outlineSunkenEnemyShips(int row, int col) for getting the direction first.
	void outlineSunkenEnemyShips(int row, int col);				    

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
	void outlineSunkenEnemyShips(int row, int col, Direction dir); 

	// Wrapper fucntion for void sinkShip(int srow, int scol, Direction dir) for getting the direction first.
	void sinkShip(int row, int col);

	// Transforms a destroyed ship to a sunken one (so that its outline can be labeled empty.
	void sinkShip(int row, int col, Direction dir);

	// The main loop of outlineSunkenEnemyShips (born from code duplication).
	void SmartPlayer::outlineLoop(int row, int col, int rowMod, int colMod, bool reverse);

	// The function pokes the coordinates adjacent to the (row,col) it is given and depening
	// On the value of outline decides it to call outlineSunkenEnemyShips(row, col , dir)  or sinkShip(row, col ,dir)
	// where dir = HORIZONAL or VERTICAL depending on the results of the poking 
	bool SmartPlayer::findDirection(int row, int col, bool outline);


public:
	SmartPlayer(){}
	~SmartPlayer(){}
	void setBoard(int player, const char **board, int numRows, int numCols) override; // called once to notify player on his board
	bool init(const string& path) override;
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
	pair<int, int> attack() override; // ask player for his move
};