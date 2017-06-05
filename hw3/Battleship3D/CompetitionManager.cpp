#include "CompetitionManager.h"
#include "Game.h"
#include <iostream>
#include <iomanip>


using namespace std;
// todo - delete all commented debug prints

void CompetitionManager::printCurrentResults(vector<PlayerGameResults>& cumulativeResults) //const
{
	static const auto generalWidth = 8;
	static const auto playerNameWidth = maxStringLength(_playersNames) + 6;

/*	lock_guard<mutex> mlock(_coutMutex);
	cout << "################################## REPORTING #########################################" << endl;*/
	
	printElement("#", generalWidth);
	printElement("Team Name", playerNameWidth);
	printElement("Wins", generalWidth);
	printElement("Losses", generalWidth);
	printElement("%", generalWidth);
	printElement("Pts For", generalWidth);
	printElement("Pts Against\n\n", generalWidth);
	auto cnt = 1;
	for (auto& gr : cumulativeResults)
	{
		printTableEntry(generalWidth, playerNameWidth, cnt++, gr);
	}
	cout << endl;
}

void CompetitionManager::reporterMethod()
{
/*	{
		std::lock_guard<std::mutex> mlock(_coutMutex);
		std::cout << "HELLO from reporter method!!!" << std::endl;
	}*/
	_pLogger->writeToLog("Reporter thread has started running");
	vector<PlayerGameResults> cumulativeResults(_numPlayers);
	vector<PlayerGameResults> sortedCumulativeResults(_numPlayers);
	for(auto i = 0; i < _numPlayers; i++)
	{
		cumulativeResults[i].ID = i;
	}

	for (auto i = 0; i < _numRounds; i++)
	{
/*		{
			std::lock_guard<std::mutex> mlock(_coutMutex);
			std::cout << "reporter waiting for round " << i+1 << std::endl;
		}*/
		_pLogger->writeToLog("Reporter is waiting for round " + to_string(i+1) + " results");
		updateCumulativeResults(cumulativeResults, i); // update round i results
		_pLogger->writeToLog("Reporter got round " + to_string(i + 1) + " results");
		// copy and sort cumulative results in descending order (by percentage)
		sortedCumulativeResults = cumulativeResults;
		sort(sortedCumulativeResults.begin(), sortedCumulativeResults.end(), greater<PlayerGameResults>());
		printCurrentResults(sortedCumulativeResults);
	}
}

void CompetitionManager::runGames(int id)
{
/*	{
		std::lock_guard<std::mutex> mlock(_coutMutex);
		std::cout << "thread " << std::this_thread::get_id() << " is running!" << std::endl;
	}*/
	_pLogger->writeToLog("Worker thread no. " + to_string(id) + " has started running");
	while (true)
	{
		// Get Game object from safe queue
		auto game = _gamesQueue.pop();
/*		{
			std::lock_guard<std::mutex> mlock(_coutMutex);
			std::cout << "thread " << std::this_thread::get_id() << " got the following game:" << std::endl;
			std::cout << "board: " << game._boardID << ", A: " << game._idA  << ", B: " << game._idB << std::endl;
		}*/
		// If the game is "poisoned" it means that the competition is over 
		// and this thread should terminate
		_pLogger->writeToLog("Worker thread no. " + to_string(id) + " got the following game: _boardID=" + to_string(game._boardID) + ", A=" + to_string(game._idA) + ", B: " + to_string(game._idB));
		if(game._boardID == -1) // "poisoned" game
		{
/*			{
				std::lock_guard<std::mutex> mlock(_coutMutex);
				std::cout << "thread " << std::this_thread::get_id() << " got a poisoned game! exiting..." << std::endl;				
			}*/
			_pLogger->writeToLog("Worker thread no. " + to_string(id) + " got a poisoned game. Returning...");
			return; 			
		}
		// Start the game
		GameRunner gameRunner(_players[game._idA], _players[game._idB], _boards[game._boardID], _pLogger);
		gameRunner.runGame();
		int roundA, roundB;
		{
			lock_guard<mutex> mlock(_mutex);
			roundA = _roundsCnt[game._idA]++;
			roundB = _roundsCnt[game._idB]++;
/*			std::lock_guard<std::mutex> mlock2(_coutMutex);
			std::cout << "thread " << std::this_thread::get_id() << " got round count:" << std::endl;
			std::cout << "A: " << roundA+1 << ", B: " << roundB+1 << std::endl;*/
		}

		// Retrieve the game results
		auto AWon = gameRunner.didAWin() ? 1 : 0;
		auto BWon = gameRunner.didBWin() ? 1 : 0;
		auto scoreA = gameRunner.getAScore();
		auto scoreB = gameRunner.getBScore();
		
		// Insert game results to the results table.
		_resultsTable.updateTable(roundA, PlayerGameResults(game._idA, AWon, 1 - AWon, scoreA, scoreB, AWon * 100));
		_resultsTable.updateTable(roundB, PlayerGameResults(game._idB, BWon, 1 - BWon, scoreA, scoreB, BWon * 100));


	}
	
}

void CompetitionManager::updateCumulativeResults(vector<PlayerGameResults>& cumulativeResults, int round)
{
	const auto& roundResults = _resultsTable.getRoundResults(round);
	for(auto& gr : roundResults)
	{
		cumulativeResults[gr.ID] += gr;
	}
}

size_t CompetitionManager::factorial(size_t n)
{

	auto res = 1;
	for (auto i = 1; i <= n; i++)
	{
		res *= i;
	}
	return res;
}

void CompetitionManager::printTableEntry(size_t generalWidth, size_t playerNameWidth, int index, PlayerGameResults& gr)
{
	printElement(to_string(index) + ".", generalWidth);
	printElement(_playersNames[gr.ID], playerNameWidth);
	printElement(gr.wins, generalWidth);
	printElement(gr.losses, generalWidth);
	cout << left << setw(generalWidth) << setfill(' ') << fixed << setprecision(2) << gr.percentage;
	printElement(gr.ptsFor, generalWidth);
	printElement(gr.ptsAgainst, generalWidth);
	cout << endl;
}

CompetitionManager::CompetitionManager(std::vector<MyBoardData>& gameBoards, std::vector<GetAlgoFuncType>& players, 
						std::vector<std::string>& playersNames, std::shared_ptr<Logger> pLogger, size_t numThreads)
	: _numThreads(numThreads), _numBoards(gameBoards.size()), _numPlayers(players.size()), 
	_numRounds(2 * _numBoards * (_numPlayers - 1)),	_resultsTable(_numPlayers, _numRounds), 
	_roundsCnt(_numPlayers,0), _pLogger(pLogger)
{
	swap(_boards, gameBoards);
	swap(_players, players);
	swap(_playersNames, playersNames);
	auto numGames = calcNumGames();
	if(_numThreads > numGames)
	{
		_numThreads = numGames;
	}
	_threadsVector.resize(_numThreads);
}


void CompetitionManager::runCompetition()
{
	_pLogger->writeToLog("Initializing competition threads");
	thread reporter(&CompetitionManager::reporterMethod, this);
	for (auto i = 0; i < _numThreads; i++)
	{
		_threadsVector[i] = thread(&CompetitionManager::runGames, this, i+1);
	}
	vector<Game> games;
	
	for(auto i = 0; i < _numBoards; i++)
	{
		for(auto j = 0; j < _numPlayers; j++)
		{
			for(auto k = 0; k < _numPlayers; k++)
			{
				if (j != k)
				{
					games.push_back(Game(i,j,k));
				}
			}			
		}
	}
	srand(unsigned(time(nullptr)));
	random_shuffle(games.begin(), games.end());
	// insert games to queue'
	_pLogger->writeToLog("Inserting games to game queue");
	for(auto g : games)
	{
		_gamesQueue.push(g);
	}
	//insert "poisoned" games to let the threads know we're done:
	for (auto i = 0; i < _numThreads; i++)
	{
		_gamesQueue.push(Game());
	}
	
	_pLogger->writeToLog("Waiting for games to be played...");
	for (auto& t : _threadsVector)
	{
		t.join();
	}
	_pLogger->writeToLog("All games have been played.");
	// notify the reporter thread in case it is still waiting
	_resultsTable.notifyEndGame();	
	reporter.join();
}
