#include "CompetitionManager.h"


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
		printElement(to_string(cnt++) + ".", generalWidth);
		printElement(_playersNames[gr.ID], playerNameWidth);
		printElement(gr.wins, generalWidth);
		printElement(gr.losses, generalWidth);
		cout << left << setw(generalWidth) << setfill(' ') << fixed << setprecision(2) << gr.percentage;
		printElement(gr.ptsFor, generalWidth);
		printElement(gr.ptsAgainst, generalWidth);
		cout << endl;
	}
	cout << endl;
}

void CompetitionManager::reporterMethod()
{
/*	{
		std::lock_guard<std::mutex> mlock(_coutMutex);
		std::cout << "HELLO from reporter method!!!" << std::endl;
	}*/
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
		updateCumulativeResults(cumulativeResults, i); // update round i results
		
		// copy and sort cumulative results in descending order (by percentage)
		sortedCumulativeResults = cumulativeResults;
		sort(sortedCumulativeResults.begin(), sortedCumulativeResults.end(), greater<PlayerGameResults>());
		printCurrentResults(sortedCumulativeResults);
	}
}

void CompetitionManager::runGames()
{
/*	{
		std::lock_guard<std::mutex> mlock(_coutMutex);
		std::cout << "thread " << std::this_thread::get_id() << " is running!" << std::endl;
	}*/
	while (true)
	{
		// Get Game object from safe queue
		auto game = _gamesQueue.pop();
/*		{
			std::lock_guard<std::mutex> mlock(_coutMutex);
			std::cout << "thread " << std::this_thread::get_id() << " got the following game:" << std::endl;
			std::cout << "board: " << game.boardID << ", A: " << game.idA  << ", B: " << game.idB << std::endl;
		}*/
		// If the game is "poisoned" it means that the competition is over 
		// and this thread should terminate
		if(game.boardID == -1) // "poisoned" game
		{
/*			{
				std::lock_guard<std::mutex> mlock(_coutMutex);
				std::cout << "thread " << std::this_thread::get_id() << " got a poisoned game! exiting..." << std::endl;				
			}*/
			return; 			
		}
		GameManager gameRunner(_players[game.idA], _players[game.idB], MyBoardData(_boards[game.boardID]));
		gameRunner.runGame();
		int roundA, roundB;
		{
			lock_guard<mutex> mlock(_mutex);
			roundA = _roundsCnt[game.idA]++;
			roundB = _roundsCnt[game.idB]++;
/*			std::lock_guard<std::mutex> mlock2(_coutMutex);
			std::cout << "thread " << std::this_thread::get_id() << " got round count:" << std::endl;
			std::cout << "A: " << roundA+1 << ", B: " << roundB+1 << std::endl;*/
		}
		auto AWon = gameRunner.didAWin() ? 1 : 0;
		auto BWon = gameRunner.didBWin() ? 1 : 0;
		auto scoreA = gameRunner.getAScore();
		auto scoreB = gameRunner.getBScore();
		_resultsTable.updateTable(roundA, PlayerGameResults(game.idA, AWon, 1 - AWon, scoreA, scoreB, AWon * 100));
		_resultsTable.updateTable(roundB, PlayerGameResults(game.idB, BWon, 1 - BWon, scoreA, scoreB, AWon * 100));


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

CompetitionManager::CompetitionManager(vector<vector3D<char>>& boards, vector<GetAlgoFuncType>& players,
											vector<string>& playersNames, size_t numThreads)
	:_numThreads(numThreads), _numBoards(boards.size()), _numPlayers(players.size()),
	_numRounds(2 * _numBoards * (_numPlayers - 1)), _resultsTable(_numPlayers, _numRounds), _roundsCnt(_numPlayers,0)
{
	swap(_boards, boards);
	swap(_players, players);
	swap(_playersNames, playersNames);
	auto numGames = calcNumGames();
	if(_numThreads > numGames)
	{
		_numThreads = numGames;
	}
	_threadsVector.resize(_numThreads);
}

void CompetitionManager::runTournament()
{
	thread reporter(&CompetitionManager::reporterMethod, this);
	for (auto& t : _threadsVector)
	{
		t = thread(&CompetitionManager::runGames, this);
	}
	vector<Game> games;
	// insert games to queue'
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
	// notify the reporter thread in case it is still waiting
	_resultsTable.notifyEndGame();	
	reporter.join();
}
