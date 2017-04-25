#include "FilePlayer.h"



FilePlayer::FilePlayer()
{
}


FilePlayer::~FilePlayer()
{
}

void FilePlayer::setMoves(vector<pair<int, int>> moves)
{
	for (size_t i = 0; i < moves.size(); ++i)
	{
		//remember moves are from 1 to ROW/COL SIZE while the board is from 0 to ROW/COL SIZE -1
		// we assume that if we got here all the moves are valid
		std::pair<int, int> move = make_pair(moves[i].first - 1, moves[i].second - 1);
		this->mMovesQueue.push(move);
	}
	moves.clear();
}
