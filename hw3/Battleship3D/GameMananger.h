#pragma once
#include "BattleshipTournament.h"
#include "IBattleshipGameAlgo.h"
#include "MyBoardData.h"

class GameMananger
{
	std::unique_ptr<IBattleshipGameAlgo> _A;
	std::unique_ptr<IBattleshipGameAlgo> _B;
	MyBoardData _boardData;

public:
	GameMananger() = delete;
	explicit GameMananger(Game game);
	int runGame();
};
