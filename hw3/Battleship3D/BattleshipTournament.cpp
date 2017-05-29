#include "BattleshipTournament.h"
#include "IBattleshipGameAlgo.h"
#include "GameMananger.h"

void BattleshipTournament::reporterMethod()
{
	while(true)
	{
		
	}
}

void BattleshipTournament::runGames()
{
	while (true)
	{
		try
		{
			auto game = _gamesQueue.pop();
			GameMananger currentGameMngr(game);
			currentGameMngr.runGame();
		}
		catch (SafeQueue<Game>::IsDead&) { return; } // means the tournament is over...
		

	}
	
}

void BattleshipTournament::runTournament()
{
	std::thread reporter(&reporterMethod, this);
	for (auto& t : _threadsVector)
	{
		t = std::thread(&runGames, this);
	}

	// code code code

	_gamesQueue.kill(); // let the threads know the games production is done
	for (auto& t : _threadsVector)
	{
		t.join();
	}
	reporter.join();
}
