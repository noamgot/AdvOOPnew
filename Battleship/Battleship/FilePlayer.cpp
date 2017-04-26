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
		// we assume that if we got here all the moves are valid
		std::pair<int, int> move = make_pair(moves[i].first, moves[i].second);
		this->mMovesQueue.push(move);
	}
}

bool FilePlayer::init(const std::string & path)
{
	return false;
}
