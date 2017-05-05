#pragma once

#include "AbstractPlayer.h"


class NaivePlayer : public AbstractPlayer
{
public:
	bool init(const string& path) override;

private:
	bool allowedToAttack(int i, int j) const;

};