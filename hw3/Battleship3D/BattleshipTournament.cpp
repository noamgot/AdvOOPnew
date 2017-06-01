#include "BattleshipTournament.h"
#include "IBattleshipGameAlgo.h"
#include "GameMananger.h"
#include <iostream>
#include <iomanip>

//todo - CHANGE!!!!!
#define OFFSET 15
void BattleshipTournament::printCurrentResults() const
{
	std::cout << "#" << std::setw(OFFSET) << "Team Name" << std::setw(OFFSET) << "Wins";
	std::cout << std::setw(OFFSET) << "Losses" << std::setw(OFFSET) << "%";
	std::cout << std::setw(OFFSET) << "Pts For" << std::setw(OFFSET) << "Pts Against\n" << std::endl;

	auto cnt = 1;
	for (auto& gr : _cumulativeResults)
	{
		std::cout << cnt++ << std::setw(OFFSET) << _playersNames[gr.ID] << std::setw(OFFSET) << gr.wins;
		std::cout << std::setw(OFFSET) << gr.loses << std::setw(OFFSET) << std::fixed << std::setprecision(2) << gr.percentage;
		std::cout << std::setw(OFFSET) << gr.ptsFor << std::setw(OFFSET) << gr.ptsAgainst << std::endl;		
	}
	std::cout << std::endl;
}

void BattleshipTournament::reporterMethod()
{
	for (auto i = 1; i <= _numRounds; i++)
	{
		updateCumulativeResults(i); // update round i results
		printCurrentResults();
	}
}

void BattleshipTournament::runGames()
{
	while (true)
	{
		auto game = _gamesQueue.pop();
		if(!game.isValid) // "poisoned" game
		{
			return;
			
		}
		GameMananger currentGameMngr(game);
		currentGameMngr.runGame();
		/*try
		{
			auto game = _gamesQueue.pop();
			GameMananger currentGameMngr(game);
			currentGameMngr.runGame();
		}
		catch (SafeQueue<Game>::IsDead&) { return; } // means the tournament is over...*/
		

	}
	
}

void BattleshipTournament::updateCumulativeResults(int round)
{
	const std::vector<PlayerGameResults>& roundResults = _resultsTable.getRoundResults(round);
	for(auto& gr : roundResults)
	{
		if (round == 1)
		{
			_cumulativeResults[gr.ID] = gr;
			
		}
		else
		{
			_cumulativeResults[gr.ID] += gr;
			
		}
		// sort cumulative results in descending order (by percentage)
		std::sort(_cumulativeResults.begin(), _cumulativeResults.end(), std::greater<PlayerGameResults>());
	}
}

void BattleshipTournament::runTournament()
{
	std::thread reporter(&reporterMethod, this);
	for (auto& t : _threadsVector)
	{
		t = std::thread(&runGames, this);
	}
	// insert games to queue
	//insert "poisoned" games to let the threads know we're done:
	for (auto i = 0; i < _threadsVector.size(); i++)
	{
		_gamesQueue.push(Game());
	}

	// code code code

	//_gamesQueue.kill(); // let the threads know the games production is done
	
	
	for (auto& t : _threadsVector)
	{
		t.join();
	}
	reporter.join();
}
