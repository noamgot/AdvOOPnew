#include "BattleshipTournament.h"
#include "IBattleshipGameAlgo.h"

void BattleshipTournament::reporterMethod()
{
	while(_tournamentIsRunning)
	{
		
	}
}

void BattleshipTournament::runGames()
{
	while (_tournamentIsRunning)
	{
		try
		{
			auto game = _gamesQueue.pop();
			auto A = std::make_unique<IBattleshipGameAlgo>();
			auto B = std::make_unique<IBattleshipGameAlgo>();
		}
		catch (MyQueue<Game>::IsDead&){} // means the tournament is over...

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


	//_tournamentIsRunning = false;
	for (auto& t : _threadsVector)
	{
		t.join();
	}
	reporter.join();
}
