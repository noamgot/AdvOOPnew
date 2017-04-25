#include "Player.h"

Player::Player() : mShipsCount(DEFAULT_SHIPS_COUNT)
{
	// zero the board
	memset(this->mBoard, 0, sizeof(char)*ROW_SIZE*COL_SIZE);
}

void Player::setBoard(int player, const char **board, int numRows, int numCols)
{
	this->mPlayerNum = player;
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            this->mBoard[i][j] = board[i][j];
        }
    }

}

// get next attack from the player's moves queue
std::pair<int, int> Player::attack()
{
    if(mMovesQueue.size() > 0)
    {
        std::pair<int,int>& nextAttack(mMovesQueue.front());//= movesQueue.front();
        mMovesQueue.pop();
        return nextAttack;
    }
    return make_pair(-1,-1);
}

bool Player::init(const std::string & path)
{
	return false;
}

void Player::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{

}

void Player::setMoves(vector<pair<int, int>> moves)
{
    for (size_t i = 0; i < moves.size(); ++i)
    {
        //remember moves are from 1 to ROW/COL SIZE while the board is from 0 to ROW/COL SIZE -1
        // we assume that if we got here all the moves are valid
        std::pair<int,int> move = make_pair(moves[i].first-1, moves[i].second-1);
        this->mMovesQueue.push(move);
    }
    moves.clear();
}

bool Player::hasMoves() const
{
    return !mMovesQueue.empty();
}

//char ** Player::getBoard()
//{
//    char **retBoard = new char *[ROW_SIZE];
//    for (int i = 0; i < ROW_SIZE; ++i)
//    {
//        retBoard[i] = new char[COL_SIZE];
//        for (int j = 0; j < COL_SIZE; ++j)
//        {
//            retBoard[i][j] = this->board[i][j];
//        }
//    }
//    return retBoard;
//}

bool Player::registerHit(std::pair<int,int> coords, eShipType shipType, AttackResult& res)
{
    int i = 0;
    bool validAttack = false;
    for(; i < DEFAULT_SHIPS_COUNT; i++)
    {
        if(this->mShipsList[i].getType() == shipType)
        {
            //Make sure this coordinate belongs to this ship
            if(this->mShipsList[i].getCoordinates().count(coords) == 1)
            {
                validAttack = this->mShipsList[i].handleHit(coords, res);
                if(res == AttackResult::Sink)
                {
                    this->mShipsCount--;
                }
                break;
            }
        }
    }
    return validAttack;
}

bool Player::hasShips() const
{
    return (this->mShipsCount > 0);
}


eShipType getShipType(char c)
{
    switch (toupper(c))
    {
        case BOAT:
            return eShipType::SHIP_TYPE_B;
        case MISSLE_SHIP:
            return eShipType::SHIP_TYPE_P;
        case SUBMARINE:
            return eShipType::SHIP_TYPE_M;
        case DESTROYER:
            return eShipType::SHIP_TYPE_D;
        default: // should not get here
            return eShipType::SHIP_TYPE_ERROR;
    }
}

Ship handleShipDiscovery(int iOrig, int jOrig, char board[][COL_SIZE])
{
    int i = iOrig;
    int j = jOrig;
    int size = 0;

    std::map<std::pair<int,int>, bool> coordinates;
    char c = board[i][j];
    // we will iterate only downwards or rightwards
    do
    {
        coordinates[make_pair(i, j)] = true;
        size++;
    }
    while(++i < ROW_SIZE && board[i][j] == c); // checking downwards
    i = iOrig;
    while (++j < COL_SIZE && board[i][j] == c) // checking rightwards
    {
        coordinates[make_pair(i, j)] = true;
        size++;
    }
    eShipType type = getShipType(c);
    return Ship(size, type, coordinates);
}

void Player::initShipsList()
{
    Ship ship;
    char c;
    for (int i = 0; i < ROW_SIZE; ++i)
    {
        for (int j = 0; j < COL_SIZE; ++j)
        {
            c = this->mBoard[i][j];
            if (c != WATER)
            {
                if ((i > 0 && mBoard[i-1][j] == c) || (j > 0 && mBoard[i][j-1] == c)) // already encountered this ship
                {
                    continue;
                }
                ship = handleShipDiscovery(i,j, this->mBoard);
                this->mShipsList.push_back(ship);
            }
        }
    }
}

