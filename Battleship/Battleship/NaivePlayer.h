#pragma once

#include "AbstractPlayer.h"


class NaivePlayer : public AbstractPlayer
{
	bool allowedToAttack(int i, int j) const;
public:
	void initAttackQueue();

};