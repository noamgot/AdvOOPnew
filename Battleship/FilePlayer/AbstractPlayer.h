#pragma once

#include <queue>
#include "GameUtilities.h"


using namespace std;


class AbstractPlayer : public IBattleshipGameAlgo
{
protected:
	int mNumOfRows;
	int mNumOfCols;
	int mShipsCount; // number of living ships - starting from DEFAULT_SHIPS_COUNT
	int mPlayerNum;
	vector<vector<char>> mBoard; 
	queue<pair<int, int>> mMovesQueue;
public:
	AbstractPlayer() : mShipsCount(GameUtilities::DEFAULT_SHIPS_COUNT){}

	virtual ~AbstractPlayer() = 0; // this class is abstract!

	void setBoard(int player, const char **board, int numRows, int numCols) override; // called once to notify player about his board
	
	pair<int, int> attack() override; // ask player for his move

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override {}// notify on last move result

	bool init(const string& path) override;
};