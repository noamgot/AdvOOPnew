﻿#pragma once
#include "PlayerGameResults.h"
#include "IBattleshipGameAlgo.h"
#include "MyBoardData.h"
#include "Logger.h"
#include "AlgorithmLoader.h"
#include "GameUtilities.h"

using namespace std;
using namespace Utilities;

#define PLAYER_A 0
#define PLAYER_B 1
// a struct for the game manager - to hold information about the players
typedef struct _PlayerAttributes
{
	bool hasMoves;
	int score;
	int shipsCount;
	std::vector<Ship> shipList;
	bool won;
}PlayerAttributes;

class GameRunner
{
public:
	GameRunner() = delete;
	explicit GameRunner(const GetAlgoFuncType& getAlgoA, const GetAlgoFuncType& getAlgoB, const MyBoardData& boardData, std::shared_ptr<Logger> pLogger);
	int runGame();

	void initIndividualBoards(MyBoardData& boardA, MyBoardData& boardB) const;

	PlayerGameResults get_grA() const {	return _grA;}
	PlayerGameResults get_grB() const {	return _grB;}
	int getAScore() const;
	int getBScore() const;
	bool didAWin() const;
	bool didBWin() const;

private:
	unique_ptr<IBattleshipGameAlgo> _playerA;
	unique_ptr<IBattleshipGameAlgo> _playerB;
	PlayerAttributes _playerAttributes[2];
	MyBoardData _boardData;

	PlayerGameResults _grA;
	PlayerGameResults _grB;
	std::shared_ptr<Logger> _pLogger;

	/* helper functions called inside playThe Game */
	void handleMove(const MyBoardData& board, Coordinate& move, int &attackerNum, int &defenderNum, unique_ptr<IBattleshipGameAlgo>& A,
		unique_ptr<IBattleshipGameAlgo>& B, PlayerAttributes playerAttributesArr[]);
	void handleMiss(Coordinate& move, unique_ptr<IBattleshipGameAlgo>& A, unique_ptr<IBattleshipGameAlgo>& B, int& attackerNum);
	void handleHitOrSink(Coordinate& move, bool& validAttack, unique_ptr<IBattleshipGameAlgo>& A, unique_ptr<IBattleshipGameAlgo>& B,
		char hitChar, int attackerNum, PlayerAttributes playerAttributesArr[]);

	/* mark a ship hit to a given player, and keeping the AttackResult in res */
	bool registerHit(PlayerAttributes& playerAttributes, Coordinate coords, eShipType shipType, AttackResult& res);

	/* returns the amount of points according to the given ship type (by char) */
	int calculateSinkScore(char c);

	void changeAttacker(int& attackerNum, int& defenderNum);


	/* initializes the given PlayerAttribute struct, according to the given board*/
	void initPlayersAttributes(PlayerAttributes& playerAttributes, const MyBoardData& board);
};