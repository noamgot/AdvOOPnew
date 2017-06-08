#pragma once

#include "GameUtilities.h"
#include "GameRunner.h"
#include "SafeQueue.h"



class CompetitionManager
{
public:

	static constexpr size_t DEFAULT_NUM_THREADS = 4;

	CompetitionManager(std::vector<MyBoardData>& boards, std::vector<MyBoardData>& boardsA, std::vector<MyBoardData>& boardsB,
						std::vector<GetAlgoFuncType>& players, std::vector<std::string>& playersNames, std::shared_ptr<Logger> pLogger, size_t numThreads);
	void runCompetition();

private:
	size_t _numThreads;
	size_t _numBoards;
	size_t _numPlayers;
	size_t _numRounds;
	std::vector<std::thread> _threadsVector;
	SafeQueue<CommonUtilities::Game> _gamesQueue;	
	GameResultsTable _resultsTable;
	std::vector<std::string> _playersNames;
	std::vector<MyBoardData> _boards;
	std::vector<MyBoardData> _boardsA;
	std::vector<MyBoardData> _boardsB;
	std::vector<GetAlgoFuncType> _players;
	std::vector<int> _roundsCnt;
	std::mutex _mutex;
	std::shared_ptr<Logger> _pLogger;

	// private functions

	void produceGames(std::vector<CommonUtilities::Game>& games) const;


	void printCurrentResults(std::vector<PlayerGameResults>& cumulativeResults, int roundNum)/*const*/;
	void reporterMethod();
	void runGames(int id); // threads function

	void updateCumulativeResults(std::vector<PlayerGameResults>& cumulativeResults, int round);

	size_t calcNumGames() const	{ return 2 * _numBoards * binomialCoeff(_numPlayers, 2); }

	static size_t factorial(size_t n);
	static size_t binomialCoeff(size_t n, size_t k)
	{
		return factorial(n) / (factorial(k) * factorial(n - k));
	}

	template<typename T>
	static void printElement(T t, const size_t width)
	{
		std::cout << std::left << std::setw(width) << std::setfill(' ') << t;
	}
	void printTableEntry(size_t generalWidth, size_t playerNameWidth, int index, PlayerGameResults& gr);
	
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
	
};
