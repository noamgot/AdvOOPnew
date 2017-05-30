#pragma once

#include <vector>
#include <thread>
#include "SafeQueue.h"
#include "IBattleshipGameAlgo.h"
#include "MyBoardData.h"

//todo - it's here just for now - we'll move it later (@noam)
struct Game
{
	std::unique_ptr<IBattleshipGameAlgo> _A;
	std::unique_ptr<IBattleshipGameAlgo> _B;
	MyBoardData _board;
	bool isValid;

	Game() : isValid(false){} // "poisoned" game object
	// todo - check how to initialize...
/*	Game(std::unique_ptr<IBattleshipGameAlgo> A, std::unique_ptr<IBattleshipGameAlgo> B, MyBoardData board)
		: _A(A), _B(B), _board(board), isValid(true){}*/
};

class BattleshipTournament
{
	static constexpr size_t DEFAULT_NUM_THREADS = 4;
	std::string _path;
	size_t _numThreads;
	std::vector<std::thread> _threadsVector;
	SafeQueue<Game> _gamesQueue;
	
	void reporterMethod();
	void runGames(); // threads function

public:
	explicit BattleshipTournament(std::string path, size_t numThreads = DEFAULT_NUM_THREADS) 
						:_path(path), _numThreads(numThreads), _threadsVector(_numThreads) {}
	void runTournament();
	void insertGame(Game game) { _gamesQueue.push(game); }
	
};
