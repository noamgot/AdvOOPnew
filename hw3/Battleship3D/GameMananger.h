#pragma once
#include "CompetitionManager.h"
#include "IBattleshipGameAlgo.h"
#include "MyBoardData.h"

class GameMananger
{
public:
	GameMananger() = delete;
	explicit GameMananger(Game game) : _grA(game.idA), _grB(game.idB) {}
	int runGame();

	void initIndividualBoards(MyBoardData& boardA, MyBoardData& boardB) const;

	PlayerGameResults get_grA() const {	return _grA;}
	PlayerGameResults get_grB() const {	return _grB;}

private:
	unique_ptr<IBattleshipGameAlgo> _A;
	unique_ptr<IBattleshipGameAlgo> _B;
	MyBoardData _boardData;

	PlayerGameResults _grA;
	PlayerGameResults _grB;
};
