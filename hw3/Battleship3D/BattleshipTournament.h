#pragma once

#include <vector>
#include <thread>
#include "SafeQueue.h"
#include "IBattleshipGameAlgo.h"
#include "MyBoardData.h"
#include "GameResultsTable.h"
#include "DLLManager.h"

//todo - it's here just for now - we'll move it later (@noam)
struct Game
{
	int boardID;
	int idA;
	int idB;

	// default ctor is a "poisoned" game object
	Game() : boardID(-1), idA(-1), idB(-1){} 
	Game(int boardID_, int idA_, int idB_) : boardID(boardID_), idA(idA_), idB(idB_){}
};

struct PlayerGameResults
{
	int ID, wins, loses, ptsFor, ptsAgainst;
	//std::string name	
	double percentage;

	PlayerGameResults() : ID(0), wins(0), loses(0), ptsFor(0), ptsAgainst(0), percentage(0)	{}
	explicit PlayerGameResults(int ID_) : ID(ID_) {}

	bool operator > (const PlayerGameResults& gr) const
	{
		return percentage > gr.percentage;
	}

	PlayerGameResults& operator+=(const PlayerGameResults& rhs) 
	{

		this->wins += rhs.wins;
		this->loses += rhs.loses;
		this->ptsFor += rhs.ptsFor;
		this->ptsAgainst += rhs.ptsAgainst;
		this->percentage = 100.0 * wins / (wins + loses);
		return *this;
	}
	
};

class BattleshipTournament
{
public:

	static constexpr size_t DEFAULT_NUM_THREADS = 4;

	BattleshipTournament(std::vector<std::vector<std::vector<std::vector<char>>>>& gameBoards,
		std::vector<GetAlgoFuncType>& players, std::vector<std::string>& playersNames, size_t numThreads);
	void runTournament();
	void insertGame(Game game) { _gamesQueue.push(game); }

private:
	

	size_t _numThreads;
	size_t _numBoards;
	size_t _numPlayers;
	size_t _numRounds;
	std::vector<std::thread> _threadsVector;
	SafeQueue<Game> _gamesQueue;	
	GameResultsTable _resultsTable;
	std::vector<std::string> _playersNames;
	std::vector<std::vector<std::vector<std::vector<char>>>> _boards;
	std::vector<GetAlgoFuncType> _players;
	std::vector<int> _roundsCnt;
	std::mutex _mutex;


	void printCurrentResults(std::vector<PlayerGameResults>& cumulativeResults) const;
	void reporterMethod();
	void runGames(); // threads function

	void updateCumulativeResults(std::vector<PlayerGameResults>& cumulativeResults, int round);

	int calcNumGames() const
	{
		return 2 * _numBoards * binomialCoeff(_numPlayers, 2);
	}

	static int factorial(int n);
	static int binomialCoeff(int n, int k)
	{
		return factorial(n) / (factorial(k) * factorial(n - k));
	}
	
	
};
