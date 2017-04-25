#pragma once
#include "AbstractPlayer.h"
class DeterministPlayer : public AbstractPlayer
{
protected:
	std::queue<std::pair<int, int>> mMovesQueue;
public:
	DeterministPlayer();
	~DeterministPlayer();
	virtual std::pair<int, int> attack() override; // ask player for his move
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
	bool hasMoves() const; // checks if the player has more moves to play
};

