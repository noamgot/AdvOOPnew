#pragma once
#include "AbstractPlayer.h"
#include "WeightePosition.h"

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

class SmartPlayer :	public AbstractPlayer
{
protected:
	vector<vector<WeightedPosition>> mWeightedBoard;
	priority_queue < WeightedPosition> mWeightedMovesQueue;
	vector<WeightedPosition> mValidMoves;
	bool isNearAShip(int row, int col);
	void updateWeights(int row, int col, AttackResult res);
	void updateWeightArounHit(int row, int col, Direction dir, AttackResult res);
	void createPriorityQueue();
	void smartSetWeight(int row, int col, float val);
	vector<Ship> mShipsList;
	

public:
	SmartPlayer();
	~SmartPlayer();
	void setBoard(int player, const char **board, int numRows, int numCols) override; // called once to notify player on his board
	bool init(const string& path) override;
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
	pair<int, int> attack() override; // ask player for his move
	bool hasMoves() const; // checks if the player has more moves to play
};