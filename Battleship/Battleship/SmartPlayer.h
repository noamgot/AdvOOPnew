#pragma once
#include "AbstractPlayer.h"
#include "WeightePosition.h"

class SmartPlayer :	public AbstractPlayer
{
protected:
	std::vector<std::vector<WeightedPosition>> mWeightedBoard;
	std::priority_queue < WeightedPosition> mWeightedMovesQueue;
	std::vector<WeightedPosition> mValidMoves;
	bool isNearAShip(int row, int col);
	void SmartPlayer::updateWeights(int row, int col, AttackResult res);
	void SmartPlayer::updateWeightArounHit(int row, int col, Direction dir, AttackResult res);
	void createPriorityQueue();
	void smartSetWeight(int row, int col, float val);
	

public:
	SmartPlayer();
	~SmartPlayer();
	virtual void setBoard(int player, const char **board, int numRows, int numCols) override; // called once to notify player on his board
	virtual bool init(const std::string& path) override;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
	virtual std::pair<int, int> attack() override; // ask player for his move
	virtual bool hasMoves() const override; // checks if the player has more moves to play
};

