#include "FilePlayer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

const string FilePlayer::ATTACK_FILE_SUFFIX(".attack");

FilePlayer::FilePlayer()
{
}


FilePlayer::~FilePlayer()
{
}

// file player doesn't care about the board, hence we do not copy it
void FilePlayer::setBoard(int player, const char** board, int numRows, int numCols)
{
	mNumOfRows = numRows;
	mNumOfCols = numCols;
	mPlayerNum = player;
}

int FilePlayer::loadMovesFromFile(const string attackFilePath)
{
	string line;
	char nextChr;
	int x, y;
	ifstream atkFile(attackFilePath);
	if (!atkFile.is_open())
	{
		cout << "Error: opening attack file failed" << endl;
		return -1;
	}
	while (getline(atkFile, line))
	{
		if (line == "")
		{
			continue;
		}
		if (line.back() == '\r')
		{
			line.back() = ' ';
		}
		x = -1;
		y = -1;
		stringstream lineStream(line);
		lineStream >> x;  //read x coor
		if (x < 1 || x > mNumOfRows)
		{
			continue;
		}

		while (lineStream >> nextChr && nextChr == ' ') {} //seek comma

		if (lineStream.eof() || nextChr != ',')
		{
			continue;
		}

		lineStream >> y; //read y coor
		if (y < 1 || y > mNumOfCols)
		{
			continue;
		}
		mMovesQueue.push(make_pair(x, y));
	}
	atkFile.close();
	return 0;
	
}

//void FilePlayer::setBoard(int player, const char ** board, int numRows, int numCols) {}

bool FilePlayer::init(const string & path)
{
	if (!AbstractPlayer::init(path))
	{
		return false;
	}
	string attackFilePath;
	auto fileNotFound = true;
	if (GameUtilities::findFileBySuffix(attackFilePath, path, ATTACK_FILE_SUFFIX, fileNotFound, mPlayerNum) < 0)
	{
		return false;
	}
	attackFilePath = path + "\\" + attackFilePath;
	// we have the file - now we can load the moves from it:
	if (loadMovesFromFile(attackFilePath) < 0)
	{
		return false;
	}
	return true;
}

IBattleshipGameAlgo* GetAlgorithm()
{
	IBattleshipGameAlgo *newP = new FilePlayer;
	return newP;
}