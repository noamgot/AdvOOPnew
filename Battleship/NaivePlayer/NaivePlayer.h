#pragma once

#include "AbstractPlayer.h"


class NaivePlayer : public AbstractPlayer
{
public:
	NaivePlayer() {}
	~NaivePlayer() {}
	bool init(const string& path) override;
private:
	// checks whether the coordinate i,j can have am enemy ship
	bool allowedToAttack(int i, int j) const;

};
