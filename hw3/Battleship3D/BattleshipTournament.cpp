#include "BattleshipTournament.h"
#include "IBattleshipGameAlgo.h"
#include "GameMananger.h"
#include <iostream>
#include <iomanip>

//todo - CHANGE!!!!!
#define OFFSET 15
void BattleshipTournament::printCurrentResults(std::vector<PlayerGameResults>& cumulativeResults) const
{
	std::cout << "#" << std::setw(OFFSET) << "Team Name" << std::setw(OFFSET) << "Wins";
	std::cout << std::setw(OFFSET) << "Losses" << std::setw(OFFSET) << "%";
	std::cout << std::setw(OFFSET) << "Pts For" << std::setw(OFFSET) << "Pts Against\n" << std::endl;

	auto cnt = 1;
	for (auto& gr : cumulativeResults)
	{
		std::cout << cnt++ << std::setw(OFFSET) << _playersNames[gr.ID] << std::setw(OFFSET) << gr.wins;
		std::cout << std::setw(OFFSET) << gr.loses << std::setw(OFFSET) << std::fixed << std::setprecision(2) << gr.percentage;
		std::cout << std::setw(OFFSET) << gr.ptsFor << std::setw(OFFSET) << gr.ptsAgainst << std::endl;		
	}
	std::cout << std::endl;
}

void BattleshipTournament::reporterMethod()
{
	std::vector<PlayerGameResults> cumulativeResults(_numPlayers);
	for(auto i = 0; i < _numPlayers; i++)
	{
		cumulativeResults[i].ID = i;
	}

	for (auto i = 0; i < _numRounds; i++)
	{
		updateCumulativeResults(cumulativeResults, i); // update round i results
		printCurrentResults(cumulativeResults);
	}
}

void BattleshipTournament::runGames()
{
	while (true)
	{
		auto game = _gamesQueue.pop();
		if(game.boardID == -1) // "poisoned" game
		{
			return; 
			
		}
		GameMananger currentGameMngr(game);
		currentGameMngr.runGame();
		int roundA, roundB;
		{
			std::lock_guard<std::mutex> mlock(_mutex);
			roundA = _roundsCnt[game.idA]++;
			roundB = _roundsCnt[game.idB]++;
		}
		_resultsTable.updateTable(roundA, currentGameMngr.get_grA());
		_resultsTable.updateTable(roundB, currentGameMngr.get_grB());	

	}
	
}

void BattleshipTournament::updateCumulativeResults(std::vector<PlayerGameResults>& cumulativeResults, int round)
{
	const std::vector<PlayerGameResults>& roundResults = _resultsTable.getRoundResults(round);
	for(auto& gr : roundResults)
	{
		cumulativeResults[gr.ID] += gr;
		// sort cumulative results in descending order (by percentage)
		std::sort(cumulativeResults.begin(), cumulativeResults.end(), std::greater<PlayerGameResults>());
	}
}

int BattleshipTournament::factorial(int n)
{

	auto res = 1;
	for (auto i = 1; i <= n; i++)
	{
		res *= i;
	}
	return res;
}

BattleshipTournament::BattleshipTournament(std::vector<std::vector<std::vector<std::vector<char>>>>& boards, std::vector<GetAlgoFuncType>& players,
											std::vector<std::string>& playersNames, size_t numThreads)
	:_numThreads(numThreads), _numBoards(boards.size()), _numPlayers(players.size()),
	_numRounds(2 * _numBoards * (_numPlayers - 1)), _resultsTable(_numPlayers, _numRounds), _roundsCnt(_numPlayers,0)
{
	std::swap(_boards, boards);
	std::swap(_players, players);
	std::swap(_playersNames, playersNames);
	auto numGames = calcNumGames();
	if(_numThreads > numGames)
	{
		_numThreads = numGames;
	}
	_threadsVector.resize(_numThreads);
}

void BattleshipTournament::runTournament()
{
	std::thread reporter(&reporterMethod, this);
	for (auto& t : _threadsVector)
	{
		t = std::thread(&runGames, this);
	}
	std::vector<Game> games;
	// insert games to queue'
	for(auto i = 0; i < _numBoards; i++)
	{
		for(auto j = 0; j < _numPlayers; j++)
		{
			for(auto k = 0; k < _numPlayers; k++)
			{
				if (j != k)
				{
					games.push_back(Game(i, j, k));
				}
			}			
		}
	}
	std::srand(unsigned(std::time(nullptr)));
	std::random_shuffle(games.begin(), games.end());
	for(auto g : games)
	{
		_gamesQueue.push(g);
	}
	//insert "poisoned" games to let the threads know we're done:
	for (auto i = 0; i < _numThreads; i++)
	{
		_gamesQueue.push(Game());
	}
	
	
	for (auto& t : _threadsVector)
	{
		t.join();
	}
	reporter.join();
}
