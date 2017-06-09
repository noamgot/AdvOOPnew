#pragma once
#include "IBattleshipGameAlgo.h"
#include "CommonUtilities.h"
#include "PlayerGameResults.h"
#include "AlgorithmLoader.h"
#include "MyBoardData.h"
#include "Logger.h"


class GameRunner
{
public:
	GameRunner() = delete;
	GameRunner(const CommonUtilities::Game& game, const GetAlgoFuncType& getAlgoA, const GetAlgoFuncType& getAlgoB, 
			const MyBoardData& boardData, const MyBoardData& boardA, const MyBoardData& boardB, std::shared_ptr<Logger> pLogger);

	// block copy operations
	GameRunner(const GameRunner& other) = delete;
	GameRunner& operator=(const GameRunner& other) = delete;

	void runGame();

	PlayerGameResults get_grA() const {	return _grA;}
	PlayerGameResults get_grB() const {	return _grB;}

private:

	std::unique_ptr<IBattleshipGameAlgo> _playerA;
	std::unique_ptr<IBattleshipGameAlgo> _playerB;
	MyBoardData _boardData;
	MyBoardData _boardA;
	MyBoardData _boardB;

	// a struct for the game manager - to hold information about the players
	struct PlayerAttributes
	{
		bool hasMoves;
		int score;
		size_t shipsCount;
		std::vector<Ship> shipList;
		//size_t movesCnt;
	} _playerAttributes[2];

	PlayerGameResults _grA;
	PlayerGameResults _grB;
	std::shared_ptr<Logger> _pLogger;
	CommonUtilities::Game _game;
	int _attackerNum;
	int _defenderNum;
	
	/* helper functions called inside playThe Game */
	void handleMove(Coordinate& move);
	void handleMiss(Coordinate& move) const;
	void handleHitOrSink(Coordinate& move, bool& validAttack, char hitChar);

	/* mark a ship hit to a given player, and keeping the AttackResult in res */
	bool registerHit(int hitPlayer, Coordinate coords, Ship::eShipType shipType, AttackResult& res);

	/* returns the amount of points according to the given ship type (by char) */
	static int calculateSinkScore(char c);

	/* changing attacker and defender indices */
	void changeAttacker()
	{
		_attackerNum ^= 1;
		_defenderNum ^= 1;		
	}


	/* initializes the given PlayerAttribute struct, according to the given board*/
	void initPlayersAttributes(const int player_id);

	/* sets the given player index and board */
	static void initPlayer(IBattleshipGameAlgo* pPlayer, int playerNum, const BoardData& board)
	{
		pPlayer->setPlayer(playerNum);
		pPlayer->setBoard(board);
		//_playerAttributes[playerNum]->
	}

	std::string winnerConclusionStr() const;
	void processGameResults();
};
