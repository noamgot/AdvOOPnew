#pragma once

#include <queue>
#include <algorithm>
#include "IBattleshipGameAlgo.h"
#include "Ship.h"	

//using namespace std;
#define ROW_SIZE 10
#define COL_SIZE 10

enum class Direction
{
	VERTICAL,
	HORIZONAL,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
};

class AbstractPlayer : public IBattleshipGameAlgo
{
protected:
	int mNumOfRows;
	int mNumOfCols;
	int mShipsCount; // number of living ships - starting from DEFAULT_SHIPS_COUNT
	int mPlayerNum;
	std::vector<std::vector<char>> mBoard; 
	std::queue<std::pair<int, int>> mMovesQueue;
	std::vector<Ship> mShipsList;
	//int score = 0;
public:
	AbstractPlayer();
	virtual ~AbstractPlayer() = 0; // this class is abstract!

	virtual void setBoard(int player, const char **board, int numRows, int numCols) override; // called once to notify player on his board
	
	virtual std::pair<int, int> attack() override; // ask player for his move
	
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result

	virtual bool init(const std::string& path) override;

	//Updates the ship that got hit (or sank).
	bool registerHit(std::pair<int,int> coords, eShipType shipType, AttackResult& res); 

	bool hasShips() const; // checks if the player has more living ships

	//char ** getBoard();

	
	virtual bool hasMoves() const; // checks if the player has more moves to play

private:

	void initShipsList();
	// a helper function to the initShipList method
	static Ship handleShipDiscovery(int iOrig, int jOrig, std::vector<std::vector<char>>& board);
};


