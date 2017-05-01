#pragma once

#include <queue>
#include <algorithm>
#include "IBattleshipGameAlgo.h"

const char WATER = ' ';
const int DEFAULT_SHIPS_COUNT = 5;

using namespace std;

enum class Direction
{
	VERTICAL,
	HORIZONAL,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
};

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
	AbstractPlayer();
	virtual ~AbstractPlayer() = 0; // this class is abstract!

	void setBoard(int player, const char **board, int numRows, int numCols) override; // called once to notify player on his board

	pair<int, int> attack() override; // ask player for his move

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result

	bool init(const std::string& path) override;

	/* return true iff the player has more ships alive*/
	bool hasShips() const;

	/* return true iff the player has more moves to play */
	virtual bool hasMoves() const;
};


