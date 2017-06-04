#pragma once

#include <queue>
#include "GameUtilities.h"
#include "MyBoardData.h"

class AbstractPlayer : public IBattleshipGameAlgo
{
protected:
	int mRows;
	int mCols;
	int mDepth;
	int mShipsCount; // number of living ships - starting from DEFAULT_SHIPS_COUNT
	int mPlayerNum;
	MyBoardData mBoard;
	std::deque<Coordinate> mMovesQueue;

	// this class should not be instantiated in the game manager - hence the ctor & dtor are protected
	AbstractPlayer() : mShipsCount(GameUtilities::DEFAULT_SHIPS_COUNT) , mBoard(){}
	~AbstractPlayer() {}
public:

	void setBoard(const BoardData& board) override; // called once to notify player about his board
	
	Coordinate attack() override; // ask player for his move

	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override {}// notify on last move result

	
};