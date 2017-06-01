#pragma once

#include <vector>
#include <thread>
#include "SafeQueue.h"
#include "IBattleshipGameAlgo.h"
#include "MyBoardData.h"
#include "GameResultsTable.h"

//todo - it's here just for now - we'll move it later (@noam)
struct Game
{
	std::unique_ptr<IBattleshipGameAlgo> _A;
	std::unique_ptr<IBattleshipGameAlgo> _B;
	std::vector<std::vector<std::vector<char>>> _board;
	bool isValid;

	Game() : isValid(false){} // "poisoned" game object
	// todo - check how to initialize...
	Game(IBattleshipGameAlgo *A, IBattleshipGameAlgo *B, std::vector<std::vector<std::vector<char>>> board)
		: _A(A), _B(B), _board(board), isValid(true){}
};

struct PlayerGameResults
{
	int ID, wins, loses, ptsFor, ptsAgainst;
	//std::string name	
	double percentage;

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
	explicit BattleshipTournament(std::string path, size_t numThreads = DEFAULT_NUM_THREADS) 
						:_path(path), _numThreads(numThreads), _threadsVector(_numThreads) {}
	void runTournament();
	void insertGame(Game game) { _gamesQueue.push(game); }

private:
	int _numPlayers;
	int _numRounds;
	static constexpr size_t DEFAULT_NUM_THREADS = 4;
	std::string _path;
	size_t _numThreads;
	std::vector<std::thread> _threadsVector;
	SafeQueue<Game> _gamesQueue;
	std::vector<PlayerGameResults> _cumulativeResults;
	GameResultsTable _resultsTable;
	std::vector<std::string> _playersNames;


	void printCurrentResults() const;
	void reporterMethod();
	void runGames(); // threads function

	void updateCumulativeResults(int round);
	
};
