#pragma once

#include "DeterministPlayer.h"


class NaivePlayer : public DeterministPlayer
{
	bool allowedToAttack(int i, int j) const;
public:
	void initAttackQueue();

};