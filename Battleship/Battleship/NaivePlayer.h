#pragma once

#include "Player.h"


class NaivePlayer : Player
{
	bool allowedToAttack(int i, int j) const;
public:
	void initAttackQueue();

};