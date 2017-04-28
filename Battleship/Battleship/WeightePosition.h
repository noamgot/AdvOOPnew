#pragma once
#include <utility>

class WeightedPosition
{
protected:
	std::pair<int, int> mPos;
	float mWeight;
public:
	WeightedPosition(std::pair<int, int> &pos, float weight);
	std::pair<int, int> getPos();
	float getWeight();
	void setWeight(float weight); 
	void setPosition(int row, int col);
	bool operator<(const WeightedPosition&  a) const;

};