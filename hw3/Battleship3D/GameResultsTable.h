#pragma once
#include "CommonUtilities.h"
#include "PlayerGameResults.h"


// todo - consider being friend class with CompetitionManager

class GameResultsTable
{
public:
	GameResultsTable() = default;
	GameResultsTable(size_t numPlayers, size_t numRounds) : _table(numRounds), _numPlayers(numPlayers){}
	

	// block copy and copy assignments ctors
	GameResultsTable(const GameResultsTable& other) = delete;
	GameResultsTable& operator=(const GameResultsTable& other) = delete;


	/* the wanted round results*/
	const std::vector<PlayerGameResults>& getRoundResults(int round);
	void updateTable(int round, PlayerGameResults gameResults);
	
	// with this function we let the thread which waits on _cv that the game is over (so it won't get stuck)
	void notifyEndGame()
	{
		_cv.notify_one();
	}

private:
	CommonUtilities::vector2D<PlayerGameResults> _table;
	std::mutex _mutex;
	std::condition_variable _cv;
	size_t _numPlayers;
};
