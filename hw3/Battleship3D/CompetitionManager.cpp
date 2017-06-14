#include "CompetitionManager.h"
#include "GameRunner.h"
#include <iostream>
#include <iomanip>

//#define printElement(t,width) /*std::left <<*/ std::setw(width) << std::setfill(' ') << t
using namespace std;
using namespace CommonUtilities;

void CompetitionManager::showConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}


void CompetitionManager::gotoxy(int x, int y)
{
	COORD coord;
	coord.X = static_cast<short>(y);
	coord.Y = static_cast<short>(x);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void CompetitionManager::printCurrentResults(vector<PlayerGameResults>& cumulativeResults, int roundNum)
{
	static const auto generalWidth = 8;
	static const auto playerNameWidth = maxStringLength(_playersNames) + 6;
	static auto firstTime = true;
	gotoxy(2, 0);
	cout << centeredStr("$$$   ROUND " + to_string(roundNum) + "/" + to_string(_numRounds) +
			(roundNum == _numRounds ? " - FINAL\a" : "") + " RESULTS   $$$", generalWidth * 6 + playerNameWidth) + "\n";
	if (firstTime)
	{
		printElement("#", generalWidth);
		printElement("Team Name", playerNameWidth);
		printElement("Wins", generalWidth);
		printElement("Losses", generalWidth);
		printElement("%", generalWidth);
		printElement("Pts For", generalWidth);
		printElement("Pts Against\n\n", generalWidth);
		firstTime = false;
	}
	gotoxy(5, 0);
	auto cnt = 1;
	for (auto& gr : cumulativeResults)
	{
		printTableEntry(generalWidth, playerNameWidth, cnt++, gr);
	}
	cout << endl;
}

void CompetitionManager::reporterMethod()
{
	_pLogger->writeToLog("Reporter thread has started running");
	vector<PlayerGameResults> cumulativeResults(_numPlayers);
	vector<PlayerGameResults> sortedCumulativeResults(_numPlayers);
	for(auto i = 0; i < _numPlayers; i++)
	{
		cumulativeResults[i].ID = i;
	}

	showConsoleCursor(false);
	for (auto i = 0; i < _numRounds; i++)
	{
		_pLogger->writeToLog("Reporter is waiting for round " + to_string(i+1) + " results");
		updateCumulativeResults(cumulativeResults, i); // update round i results
		_pLogger->writeToLog("Reporter got round " + to_string(i + 1) + " results");
		// copy and sort cumulative results in descending order (by percentage)
		sortedCumulativeResults = cumulativeResults;
		sort(sortedCumulativeResults.begin(), sortedCumulativeResults.end(), greater<PlayerGameResults>());
		printCurrentResults(sortedCumulativeResults, i+1);
	}
	showConsoleCursor(true);
}

void CompetitionManager::runGames(int id)
{
	_pLogger->writeToLog("Worker thread no. " + to_string(id) + " has started running");
	while (true)
	{
		// Get Game object from safe queue
		auto game = _gamesQueue.pop();

		// If the game is "poisoned" it means that the competition is over 
		// and this thread should terminate
		if(game._boardID == -1) // "poisoned" game
		{
			_pLogger->writeToLog("Worker thread no. " + to_string(id) + " got a poisoned game. Returning...");
			return; 			
		}

		_pLogger->writeToLog("Worker thread no. " + to_string(id) + " got the following game: boardID=" + to_string(game._boardID) + 
								", A=" + to_string(game._idA) + ", B=" + to_string(game._idB));
		
		GameRunner gameRunner(game, _players[game._idA], _players[game._idB], _boards[game._boardID],
								_boardsA[game._boardID], _boardsB[game._boardID],  _pLogger);
		// Run the game!
		gameRunner.runGame();

		// note that _roundCnt is a vector of *atomic* ints - no need for mutex!
		auto roundA = _roundsCnt[game._idA]++;
		auto roundB = _roundsCnt[game._idB]++;
/*		_pLogger->writeToLog("For game: boardID=" + to_string(game._boardID) + ", A=" + to_string(game._idA) + ", B=" + to_string(game._idB) +
			" Got rounds count: A-" + to_string(roundA) + ", B-" + to_string(roundB));*/
	
		// Insert game results to the results table.
		_resultsTable.updateTable(roundA, gameRunner.get_grA());
		_resultsTable.updateTable(roundB, gameRunner.get_grB());
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

void CompetitionManager::printTableEntry(size_t generalWidth, size_t playerNameWidth, int index, const PlayerGameResults& gr)
{
	printElement(to_string(index) + ".", generalWidth);
	printElement(_playersNames[gr.ID], playerNameWidth);
	printElement(gr.wins, generalWidth);
	printElement(gr.losses, generalWidth);
	cout << left << setw(generalWidth) << setfill(' ') << fixed << setprecision(2) << gr.percentage;
	printElement(gr.ptsFor, generalWidth);
	printElement(gr.ptsAgainst, generalWidth);
	cout << "\n";
}

void CompetitionManager::logBoardsAndPlayers() const
{
	_pLogger->writeToLog("************************************");
	_pLogger->writeToLog("*   LET THE COMPETITION BEGIN!!!   *");
	_pLogger->writeToLog("************************************");
	_pLogger->writeToLog("\t$$$ BOARDS ID TABLE: $$$");
	for (auto i = 0; i < _boardsNames.size(); i++)
	{
		_pLogger->writeToLog("\t" + to_string(i) + " == " + _boardsNames[i]);
	}
	_pLogger->writeToLog("\t$$$ PLAYERS ID TABLE: $$$");
	for (auto i = 0; i < _playersNames.size(); i++)
	{
		_pLogger->writeToLog("\t" + to_string(i) + " == " + _playersNames[i]);
	}
	_pLogger->writeToLog("************************************");
	
}


CompetitionManager::CompetitionManager(vector<MyBoardData>& boards, vector<MyBoardData>& boardsA, vector<MyBoardData>& boardsB,
						vector<GetAlgoFuncType>& players, vector<string>& playersNames, vector<string>& boardsNames,
						shared_ptr<Logger> pLogger, size_t numThreads) :
	_boards(move(boards)), _boardsA(move(boardsA)), _boardsB(move(boardsB)), _players(move(players)), _playersNames(move(playersNames)), 
	_boardsNames(move(boardsNames)), _numThreads(numThreads), _numBoards(boards.size()), _numPlayers(players.size()),
	_numRounds(2 * _numBoards * (_numPlayers - 1)), _resultsTable(_numPlayers, _numRounds),	_roundsCnt(_numPlayers), _pLogger(pLogger)/*, _gamesPlayed(0)*/
{
	auto numGames = calcNumGames();
	if (_numThreads > numGames)
	{
		_numThreads = numGames;
	}
	_threadsVector.resize(_numThreads);
}

void CompetitionManager::runCompetition()
{
	printOpeningMessage();
	_pLogger->writeToLog("Number of legal players: " + to_string(_players.size()), true);
	_pLogger->writeToLog("Number of legal boards: " + to_string(_boards.size()), true);
	logBoardsAndPlayers();
	_pLogger->writeToLog("Initializing competition threads");
	
	thread reporter(&CompetitionManager::reporterMethod, this);
	for (auto i = 0; i < _numThreads; i++)
	{
		_threadsVector[i] = thread(&CompetitionManager::runGames, this, i+1);
	}
	
	// insert games to queue
	_pLogger->writeToLog("Inserting games to game queue");
	// for each board we put a balanced game order in the queue
	for (auto k = 0; k < _numBoards; k++)
	{
		for (auto i = 1; i < _numPlayers; ++i)
		{
			for (auto j = 0; j < _numPlayers; ++j)
			{
				_gamesQueue.push(Game(k, j, (j + i) % _numPlayers));
			}
		
		}
	}

	//insert "poisoned" games to let the threads know we're done:
	for (auto i = 0; i < _numThreads; i++)
	{
		_gamesQueue.push(Game(-1,-1,-1));
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

void CompetitionManager::printOpeningMessage()
{
	system("cls");
	cout << "********************************\n" << 
		"*                              *\n" << 
		"*        BATTLESHIP-3D         *\n" << 
		"*                              *\n" << 
		"*    Version 3.0, June 2017    *\n" << 
		"*                              *\n" << 
		"*         Created by:          *\n" << 
		"*         Ben Ohayon           *\n" << 
		"*         Uri Bracha           *\n" << 
		"*        Noam Gottlieb         *\n" << 
		"*                              *\n" << 
		"*    (c) all rights reserved   *\n" << 
		"*                              *\n" << 
		"********************************";
	Sleep(3000);
	system("cls");
}
