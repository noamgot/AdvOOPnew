#pragma once
#include "IBattleshipGameAlgo.h"
#include "GameUtilities.h"
#include "MyBoardData.h"
#include "Logger.h"


#define PLAYER_A 0
#define PLAYER_B 1
#define PLAYER_COUNT 2
// a struct for the game manager - to hold information about the players
typedef struct _PlayerAttributes
{
	bool hasMoves;
	int score;
	size_t shipsCount;
	std::vector<Ship> shipList;
	int won;
}PlayerAttributes;

class GameRunner
{
public:
	GameRunner() = delete;
	explicit GameRunner(const CommonUtilities::Game& game, const GetAlgoFuncType& getAlgoA, const GetAlgoFuncType& getAlgoB, 
			const MyBoardData& boardData, const MyBoardData& boardA, const MyBoardData& boardB, Logger  *pLogger);
	
	std::vector<PlayerGameResults> runGame();
	PlayerGameResults get_grA() const {	return _grA;}
	PlayerGameResults get_grB() const {	return _grB;}
	//int getAScore() const;
	//int getBScore() const;
	//bool didAWin() const;
	//bool didBWin() const;

private:
	std::unique_ptr<IBattleshipGameAlgo> _playerA;
	std::unique_ptr<IBattleshipGameAlgo> _playerB;
	PlayerAttributes _playerAttributes[2];
	MyBoardData _boardData;
	MyBoardData _boardA;
	MyBoardData _boardB;

	PlayerGameResults _grA;
	PlayerGameResults _grB;
	Logger *_pLogger;
	CommonUtilities::Game _game;

	/* helper functions called inside playThe Game */
	void handleMove(const MyBoardData& board, Coordinate& move, int &attackerNum, int &defenderNum, IBattleshipGameAlgo* A,
	                IBattleshipGameAlgo* B, PlayerAttributes playerAttributesArr[]);
	void handleMiss(Coordinate& move, int attackerNum) const;
	void handleHitOrSink(Coordinate& move, bool& validAttack,
	                     char hitChar, int attackerNum, PlayerAttributes playerAttributesArr[]);

	/* mark a ship hit to a given player, and keeping the AttackResult in res */
	bool registerHit(PlayerAttributes& playerAttributes, Coordinate coords, eShipType shipType, AttackResult& res);

	/* returns the amount of points according to the given ship type (by char) */
	static int calculateSinkScore(char c);

	static void changeAttacker(int& attackerNum, int& defenderNum)
	{
		attackerNum ^= 1;
		defenderNum ^= 1;
	}

	void AndTheWinnerIs();
	/* initializes the given PlayerAttribute struct, according to the given board*/
	void initPlayersAttributes(const MyBoardData& board, const int player_id);
};
