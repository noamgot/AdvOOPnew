#pragma once
#include "CompetitionManager.h"
#include "GameManagerUtilities.h"
#include "IBattleshipGameAlgo.h"
#include "MyBoardData.h"
#include "Logger.h"

class GameManager
{
public:
	GameManager() = delete;
	explicit GameManager(GetAlgoFuncType getAlgoA, GetAlgoFuncType getAlgoB, MyBoardData boardData);
	GameMananger() = delete;
	explicit GameMananger(Game game, std::shared_ptr<Logger> pLogger) : _grA(game.idA), _grB(game.idB), _pLogger(pLogger) {}
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
	GameManagerUtilities::PlayerAttributes _playerAttributes[2];
	MyBoardData _boardData;

	PlayerGameResults _grA;
	PlayerGameResults _grB;
	std::shared_ptr<Logger> _pLogger;
};
