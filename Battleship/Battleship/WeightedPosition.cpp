#include "WeightePosition.h"

WeightedPosition::WeightedPosition(std::pair<int, int> &pos, float weight) : 
	mWeight(weight),
	mPos(pos) 
{}

void WeightedPosition::setWeight(float weight)
{
	this->mWeight = weight;
}

void WeightedPosition::setPosition(int row, int col)
{
	this->mPos = std::make_pair(row, col);
}


std::pair<int, int> WeightedPosition::getPos()
{
	return this->mPos;
}

float WeightedPosition::getWeight()
{
	return this->mWeight;
}

bool WeightedPosition::operator<(const WeightedPosition&  rhs) const { return mWeight < rhs.mWeight;}
