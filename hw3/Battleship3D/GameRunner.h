#pragma once
#include "CompetitionManager.h"
#include "GameUtilities.h"
#include "IBattleshipGameAlgo.h"
#include "MyBoardData.h"

class GameRunner
{
public:
	GameRunner() = delete;
	explicit GameRunner(GetAlgoFuncType getAlgoA, GetAlgoFuncType getAlgoB, MyBoardData boardData);
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
	GameUtilities::PlayerAttributes _playerAttributes[2];
	MyBoardData _boardData;


	PlayerGameResults _grA;
	PlayerGameResults _grB;
};
