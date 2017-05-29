#pragma once

#include <vector>
#include <thread>
#include "SafeQueue.h"

//todo - it's here just for now - we'll move it later (@noam)
struct Game
{
	std::string dll1Path;
	std::string dll2Path;
	std::string boardPath;

	Game() = default;
	Game(std::string dll1Path_, std::string dll2Path_, std::string boardPath_)
		: dll1Path(dll1Path_), dll2Path(dll2Path_), boardPath(boardPath_){}
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
