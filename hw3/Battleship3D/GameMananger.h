#pragma once
#include "CompetitionManager.h"
#include "IBattleshipGameAlgo.h"
#include "MyBoardData.h"
#include "Logger.h"

class GameMananger
{
public:
	GameMananger() = delete;
	explicit GameMananger(Game game, std::shared_ptr<Logger> pLogger) : _grA(game.idA), _grB(game.idB), _pLogger(pLogger) {}
	int runGame();

	void initIndividualBoards(MyBoardData& boardA, MyBoardData& boardB) const;

	PlayerGameResults get_grA() const {	return _grA;}
	PlayerGameResults get_grB() const {	return _grB;}

private:
	std::unique_ptr<IBattleshipGameAlgo> _A;
	std::unique_ptr<IBattleshipGameAlgo> _B;
	MyBoardData _boardData;
	PlayerGameResults _grA;
	PlayerGameResults _grB;
	std::shared_ptr<Logger> _pLogger;
};
