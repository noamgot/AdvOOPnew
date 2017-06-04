#pragma once
#include <vector>
#include "GameUtilities.h"
#include <thread>
#include "SafeQueue.h"
#include "GameRunner.h"
#include "GameResultsTable.h"
#include "Game.h"
#include <iostream>
#include <iomanip>



template <typename T>
using vector2D = vector<vector<T>>;
template <typename T>
using vector3D =  vector<vector<vector<T>>>;

class CompetitionManager
{
public:

	static constexpr size_t DEFAULT_NUM_THREADS = 4;

	CompetitionManager(vector<vector3D<char>>& gameBoards, vector<GetAlgoFuncType>& players, vector<string>& playersNames, size_t numThreads);
	void runTournament();

private:
	

	size_t _numThreads;
	size_t _numBoards;
	size_t _numPlayers;
	size_t _numRounds;
	vector<thread> _threadsVector;
	SafeQueue<Game> _gamesQueue;	
	GameResultsTable _resultsTable;
	vector<string> _playersNames;
	vector<vector3D<char>> _boards;
	vector<GetAlgoFuncType> _players;
	vector<int> _roundsCnt;
	mutex _mutex /*, _coutMutex*/ ;


	void printCurrentResults(vector<PlayerGameResults>& cumulativeResults)/*const*/;
	void reporterMethod();
	void runGames(); // threads function

	void updateCumulativeResults(vector<PlayerGameResults>& cumulativeResults, int round);

	size_t calcNumGames() const	{ return 2 * _numBoards * binomialCoeff(_numPlayers, 2); }

	static size_t factorial(size_t n);
	static size_t binomialCoeff(size_t n, size_t k)
	{
		return factorial(n) / (factorial(k) * factorial(n - k));
	}

	template<typename T>
	static void printElement(T t, const int& width)	{ cout << left << setw(width) << setfill(' ') << t;	}
	static bool stringComp(string const& lhs, string const& rhs) { return lhs.size() < rhs.size();}
	static size_t maxStringLength(vector<string> const &lines) { return max_element(lines.begin(), lines.end(), stringComp)->size();	}
	
};
