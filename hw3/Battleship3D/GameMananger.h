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
	explicit GameMananger(Game& game) : _A(std::move(game._A)), _B(std::move(game._B)), _boardData(game._board){}
	int runGame();

	void initIndividualBoards(MyBoardData& boardA, MyBoardData& boardB) const;
};
