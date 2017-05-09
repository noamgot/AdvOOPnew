#pragma once

#include "AbstractPlayer.h"


class NaivePlayer : public AbstractPlayer
{
public:
	NaivePlayer() {}
	~NaivePlayer() {}
	bool init(const string& path) override;
private:
	bool allowedToAttack(int i, int j) const;

};
