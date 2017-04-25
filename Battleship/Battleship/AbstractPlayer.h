#ifndef ABSTRACTPLAYER_H
#define ABSTRACTPLAYER_H

#include <queue>
#include <cctype>
#include "IBattleshipGameAlgo.h"
#include "Ship.h"

using namespace std;
#define ROW_SIZE 10
#define COL_SIZE 10
#define DEFAULT_SHIPS_COUNT 5

class AbstractPlayer : public IBattleshipGameAlgo
{
protected:	
    char mBoard[ROW_SIZE][COL_SIZE];
    std::vector<Ship> mShipsList;
    int mShipsCount; // number of living ships - starting from DEFAULT_SHIPS_COUNT
	int mPlayerNum;
    //int score = 0;


public:
	AbstractPlayer();
	~AbstractPlayer();
    virtual void setBoard(int player, const char **board, int numRows, int numCols) override; // called once to notify player on his board
    
	virtual bool init(const std::string& path) override;

	//Updates the ship that got hit (or sank).
    bool registerHit(std::pair<int,int> coords, eShipType shipType, AttackResult& res); 

    bool hasShips() const; // checks if the player has more living ships

    void initShipsList();

    //char ** getBoard();
};


#endif //ABSTRACTPLAYER_H
