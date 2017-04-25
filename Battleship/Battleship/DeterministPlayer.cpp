#include "DeterministPlayer.h"



DeterministPlayer::DeterministPlayer()
{
}


DeterministPlayer::~DeterministPlayer()
{
}

// get next attack from the player's moves queue
std::pair<int, int> DeterministPlayer::attack()
{
	if (mMovesQueue.size() > 0)
	{
		std::pair<int, int>& nextAttack(mMovesQueue.front());
		mMovesQueue.pop();
		return nextAttack;
	}
	return make_pair(-1, -1);
}

void DeterministPlayer::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{

}

bool DeterministPlayer::hasMoves() const
{
	return !mMovesQueue.empty();
}
