#pragma once

#include "CommonUtilities.h"
#include "GameResultsTable.h"
#include "PlayerGameResults.h"
#include "AlgorithmLoader.h"
#include "MyBoardData.h"
#include "SafeQueue.h"
#include <atomic>


class CompetitionManager
{
public:

	static constexpr size_t DEFAULT_NUM_THREADS = 4;

	CompetitionManager(std::vector<MyBoardData>& boards, std::vector<MyBoardData>& boardsA, std::vector<MyBoardData>& boardsB,
						std::vector<GetAlgoFuncType>& players, std::vector<std::string>& playersNames, std::vector<std::string>& boardsNames, 
						std::shared_ptr<Logger> pLogger, size_t numThreads);
	
	void runCompetition();

	// a struct representing a game "recipe" - including the IDs of the board and the players
	struct Game
	{
		int _boardID;
		int _idA;
		int _idB;

		Game() = default;
		Game(int boardID_, int idA_, int idB_) : _boardID(boardID_), _idA(idA_), _idB(idB_) {}
	};

private:
	size_t _numThreads;
	size_t _numBoards;
	size_t _numPlayers;
	size_t _numRounds;
	std::vector<std::thread> _threadsVector;
	SafeQueue<Game> _gamesQueue;	
	GameResultsTable _resultsTable;
	std::vector<std::string> _playersNames;
	std::vector<std::string> _boardsNames;
	std::vector<MyBoardData> _boards;
	std::vector<MyBoardData> _boardsA;
	std::vector<MyBoardData> _boardsB;
	std::vector<GetAlgoFuncType> _players;
	std::vector<std::atomic<int>> _roundsCnt;
	std::shared_ptr<Logger> _pLogger;

	//std::atomic<int> _gamesPlayed;

	// private functions

	/* with this function the reported thread is printing the game results*/
	void printCurrentResults(std::vector<PlayerGameResults>& cumulativeResults, int roundNum);
	template<typename T>
	static void printElement(T t, const size_t width)
	{
		std::cout << std::left << std::setw(width) << std::setfill(' ') << t;
	}
	void printTableEntry(size_t generalWidth, size_t playerNameWidth, int index, const PlayerGameResults& gr);

	/* the main method of the reporter thread */
	void reporterMethod();

	/* the main method of the worker threads thread (which run games) */
	void runGames(int id); 

	void updateCumulativeResults(std::vector<PlayerGameResults>& cumulativeResults, int round);

	size_t calcNumGames() const	{ return 2 * _numBoards * binomialCoeff(_numPlayers, 2); }

	static size_t factorial(size_t n);
	static size_t binomialCoeff(size_t n, size_t k)
	{
		return factorial(n) / (factorial(k) * factorial(n - k));
	}
	
	static bool stringComp(std::string const& lhs, std::string const& rhs)
	{
		return lhs.size() < rhs.size();
	}
	
	static size_t maxStringLength(std::vector<std::string> const &lines)
	{
		return max_element(lines.begin(), lines.end(), stringComp)->size();
	}

	/* centers a string (for printing) */
	static std::string centeredStr(std::string input, size_t width) 
	{
		return std::string((width - input.length()) / 2, ' ') + input;
	}

	void logBoardsAndPlayers() const;

	static void printOpeningMessage();

	static void showConsoleCursor(bool showFlag);

	static void gotoxy(int x, int y);
	
};
