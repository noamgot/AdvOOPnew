#include "GameUtilities.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <bitset>
#include <fstream>
#include <direct.h>
#include <set>
#include <map>
#include <cctype>
#include <stdlib.h>
#include <Windows.h>
#include "CompetitionManager.h"

using namespace std;
using namespace CommonUtilities;

namespace GameUtilities
{
	int GameUtilities::processInputArguments(int argc, char** argv, string& dirPath, int& numThreads)
	{	
		char *p;
		auto gotDirPath = false;
		if (argc >= 2)
		{ 
			// we accept the arguments in any order, 
			// and we assume that if a folder path is given it is the first argument
			for (auto i = 1; i < argc; ++i)
			{
				if (string(argv[i]) == PARAM_THREADS)
				{
					if (i + 1 < argc)
					{
						int _numThreads = strtol(argv[i + 1], &p, 10);
						if (!*p && _numThreads > 0)
						{
							numThreads = _numThreads;
							break;
						}
					}
					// if there's not a valid positive after PARAM_THREADS - ignore...
				}
				else if (i == 1)
				{ // we assume that if there's a folder path it is the first argument
					dirPath = argv[1];
					gotDirPath = true;
				}
			}
		}
		if (!gotDirPath) // directory path not given
		{
			dirPath = getDirPath();
		}
		// convert the given directory path to full path
		return convertToFullPath(dirPath);
	}

	/*
	void GameUtilities::printGameResults(PlayerAttributes playerAttributesArr[])
	{
		if (playerAttributesArr[0].shipsCount <= 0)
		{
			cout << "Player B won" << endl;
		}
		else if (playerAttributesArr[1].shipsCount <= 0)
		{
			cout << "Player A won" << endl;
		}
		cout << "Points:" << endl;
		cout << "Player A: " << playerAttributesArr[0].score << endl;
		cout << "Player B: " << playerAttributesArr[1].score << endl;
	}
	*/

	int GameUtilities::printBoardErrors(bitset<4>& errShipsA, bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck)
	{
		auto ret = 0;
		// Print possible errors
		printWrongSizeOrShapeError(errShipsA, ret, "A");
		printWrongSizeOrShapeError(errShipsB, ret, "B");
		printBadShipsCountErrror(shipCountA, ret, "A");
		printBadShipsCountErrror(shipCountB, ret, "B");
		if (adjCheck)
		{
			cout << "Adjacent Ships on Board" << endl;
			ret = -1;
		}
		return ret;
	}

	void GameUtilities::printWrongSizeOrShapeError(bitset<4>& errShips, int& ret, const string player)
	{
		char shipMap[4] = { BOAT,MISSLE_SHIP,SUBMARINE,DESTROYER };
		if (player == "B")
		{
			for (auto& c : shipMap)
			{
				c = tolower(c);
			}
		}
		for (auto i = 0; i < 4; i++)
		{
			if (errShips[i])
			{
				cout << "Wrong size or shape for ship " << shipMap[i] << " for player " << player << endl;
				ret = -1;
			}
		}
	}

	void GameUtilities::printBadShipsCountErrror(int shipCount, int& ret, const string player)
	{
		if (shipCount != DEFAULT_SHIPS_COUNT)
		{
			string filler = shipCount > DEFAULT_SHIPS_COUNT ? "many" : "few";
			cout << "Too " << filler << " ships for player " << player << endl;
			ret = -1;
		}
	}

	int GameUtilities::convertToFullPath(string& dirPath)
	{
		if (dirPath == BAD_STRING) //error occurred in getDirPath()
		{
			return -1;
		}
		// get the full path of the directory
		char fullPath[MAX_PATH_LEN];
		if (!_fullpath(fullPath, dirPath.c_str(), MAX_PATH_LEN - 1))
		{
			cout << "Error: conversion of directory path to full path failed" << endl;
			return -1;
		}
		// copy the full path to dirPath
		dirPath = fullPath;
		// clear "\\" if added by _fullpath()
		if (dirPath.back() == '\\')
		{
			dirPath.pop_back();
		}
		return 0;
	}

	string GameUtilities::getDirPath()
	{
		char buff[MAX_PATH_LEN];
		if (!_getcwd(buff, MAX_PATH_LEN - 1))
		{
			return BAD_STRING; //signs the string is bad
		}
		string temp(buff);
		return temp;
	}
	
	/*
	void GameUtilities::initPlayerShipsList(const char** board, int numOfRows, int numOfCols, PlayerAttributes& playerAttributes)
	{
		for (auto i = 0; i < numOfRows; ++i)
		{
			for (auto j = 0; j < numOfCols; ++j)
			{
				auto c = board[i][j];
				if (c != WATER)
				{
					if (i > 0 && board[i - 1][j] == c || j > 0 && board[i][j - 1] == c) // already encountered this ship
					{
						continue;
					}
					auto ship = handleShipDiscovery(i, j, numOfRows, numOfCols, board);
					playerAttributes.shipList.push_back(ship);
				}
			}
		}
	}
*/
	
	Ship GameUtilities::handleShipDiscovery(int iOrig, int jOrig, int numOfRows, int numOfCols, const char** board)
	{

		auto i = iOrig;
		auto j = jOrig;
		auto size = 0;
		map<pair<int, int>, bool> coordinates;
		auto c = board[i][j];
		// we will iterate only downwards or rightwards
		do
		{
			// remember that we save the coordinates from in the form of 1 to ROW/COL SIZE (and not starting from 0)
			// hence we give a +1 offset
			coordinates[make_pair(i + 1, j + 1)] = true;
			size++;
		} while (++i < numOfRows && board[i][j] == c); // checking downwards
		i = iOrig;
		while (++j < numOfCols && board[i][j] == c) // checking rightwards
		{
			coordinates[make_pair(i + 1, j + 1)] = true;
			size++;
		}
		//return Ship(size, charToShipType(c), coordinates);
		return Ship(); // todo - change!!!!!! just for tests...
	}

	void GameUtilities::initBoards3D(const vector<string>& boardPaths, vector<vector3d>& boards)
	{
		for (string boardPath : boardPaths)
		{
			auto rows = 0, cols = 0, depth = 0;
			string line;
			ifstream boardFile(boardPath);
			if (!boardFile.is_open())
			{
				cout << "Error: opening board file failed" << endl;
				continue;
			}
			set<char> charSet{ WATER, BOAT,MISSLE_SHIP, SUBMARINE, DESTROYER, char(tolower(BOAT)),
				char(tolower(MISSLE_SHIP)), char(tolower(SUBMARINE)), char(tolower(DESTROYER)) };

			getline(boardFile, line);
			if (boardFile.eof() || boardFile.fail())
			{
				continue;
			}
			if (getDims(line, rows, cols, depth) <= 0)
			{
				continue;
			}

			RawBoard board(rows, std::vector<std::vector<char>>(cols, std::vector<char>(depth)));

			//MyBoardData board(rows, cols, depth);
			// if first line after dimensions is not empty - bad format
			getline(boardFile, line);
			if (!boardFile.eof() && !line.empty())
			{
				continue;
			}
			getline(boardFile, line);

			for (auto k = 0; k < depth ; k++)
			{
				for (auto i = 0; i < rows; i++)
				{
					for (auto j = 0; j < cols; j++)
					{
						if (boardFile.eof() || line.empty() || j >= line.size())
						{
							board[i][j][k] = WATER;
						}
						else
						{
							board[i][j][k] = charSet.find(line[j]) == charSet.end() ? WATER : line[j];
						}
					}
					if (!boardFile.eof() && !line.empty() && i != rows-1)
					{
						getline(boardFile, line);
					}
				}
				if (boardFile.eof())
				{
					continue;
				}
				while (getline(boardFile, line))
				{
					if (line.empty())
					{
						getline(boardFile, line);
						break;
					}
				}
			}

			boardFile.close();
			if (checkBoard3D(board) < 0)
			{
				continue;
			}
			boards.push_back(board);
		}
	}

	int GameUtilities::getDims(const string line, int& rows, int& cols, int& depth)
	{
		int j = 0;
		for (int i = 0; i <= 2; i++)
		{
			string buff = "";
			while (j < line.size() && line[j] != 'x' && line[j] != 'X')
			{
				if (std::isdigit(line[j]) == 0)
				{
					return -1;
				}
				buff += line[j];
				j++;
			}
			j++;
			if (i == 0)
			{
				cols = stoi(buff);
			}
			if (i == 1)
			{
				rows = stoi(buff);
			}
			if (i == 2)
			{
				depth = stoi(buff);
			}
		}
		return rows*cols*depth;
	}

	int GameUtilities::checkShape3D(RawBoard& board, const int size, int k, int i, int j)
	{
		auto rows = board.size();
		auto cols = board[0].size();
		auto depth = board[0][0].size();
		auto verL = 1, horL = 1, depL = 1;
		// run horizontally, check above, below, inwards and outwards
		while (j + horL < cols && board[i][j][k] == board[i][j + horL][k])
		{
			if ((i + 1 < rows && board[i][j][k] == board[i + 1][j + horL][k]) ||
				(i - 1 >= 0 && board[i][j][k] == board[i - 1][j + horL][k]) ||
				(k + 1 < depth && board[i][j][k] == board[i][j + horL][k + 1]) ||
				(k - 1 >= 0 && board[i][j][k] == board[i][j + horL][k - 1]))
			{
				return -1;
			}
			horL++;
		}
		// run vertically, check right, left, inwards and outwards
		while (i + verL < rows && board[i][j][k] == board[i + verL][j][k])
		{
			if ((j + 1 < cols && board[i][j][k] == board[i + verL][j + 1][k]) ||
				(j - 1 >= 0 && board[i][j][k] == board[i + verL][j - 1][k]) ||
				(k + 1 < depth && board[i][j][k] == board[i + verL][j][k + 1]) ||
				(k - 1 >= 0 && board[i][j][k] == board[i + verL][j][k - 1]))
			{
				return -1;
			}
			verL++;
		}
		// run depth, check right, left, above and below
		while (k + depL < depth && board[i][j][k] == board[i][j][k + depL])
		{
			if ((j + 1 < cols && board[i][j][k] == board[i][j + 1][k + depL]) ||
				(j - 1 >= 0 && board[i][j][k] == board[i][j - 1][k + depL]) ||
				(i + 1 < rows && board[i][j][k] == board[i + 1][j][k + depL]) ||
				(i - 1 >= 0 && board[i][j][k] == board[i - 1][j][k + depL]))
			{
				return -1;
			}
			depL++;
		}

		// check for misshape in size (no dimension matches ship size or 2 or more dimensions are longer than 1 cell)
		if ((horL != size && verL != size && depL != size) || (horL > 1 && verL > 1) || (horL > 1 && depL > 1) || (verL > 1 && depL > 1))
		{
			return -1;
		}
		return 1;
	}

	//todo - add prints to logger
	int GameUtilities::checkBoard3D(RawBoard& board)
	{
		auto rows = board.size();
		auto cols = board[0].size();
		auto depth = board[0][0].size();
		auto shipCountA = 0, shipCountB = 0, isShipA = 0, isShipB = 0, adjCheck = 0;
		map<char, int> shipsA = { { BOAT,1 },{ MISSLE_SHIP,2 },{ SUBMARINE,3 },{ DESTROYER,4 } };
		map<char, int> shipsB = { { char(tolower(BOAT)),1 },{ char(tolower(MISSLE_SHIP)),2 },
		{ char(tolower(SUBMARINE)),3 },{ char(tolower(DESTROYER)),4 } };
		bitset<4> errShipsA, errShipsB;// error flags for ship misshapes

		for (auto k = 0; k < depth; k++)
		{
			for (auto i = 0; i < rows; i++)
			{
				for (auto j = 0; j < cols; j++)
				{
					if (board[i][j][k] != ' ')
					{
						isShipA = shipsA[board[i][j][k]] != 0;       // 1 if its a ship, otherwise 0
						isShipB = shipsB[board[i][j][k]] != 0;
						if (isShipA || isShipB)
						{
							// check if its new
							if (!(k != 0 && board[i][j][k - 1] == board[i][j][k] ||
								i != 0 && board[i - 1][j][k] == board[i][j][k] ||
								j != 0 && board[i][j - 1][k] == board[i][j][k]))
							{
								// check for misshape
								if (checkShape3D(board, shipsB[tolower(board[i][j][k])], k, i, j) < 0)
								{
									if (isShipA)
									{
										errShipsA[shipsA[board[i][j][k]] - 1] = 1;
									}
									if (isShipB)
									{
										errShipsB[shipsB[board[i][j][k]] - 1] = 1;
									}
									return -1;
								}
								shipCountA += isShipA;
								shipCountB += isShipB;
							}
							// Check if any adjacent ships exist
							if (k != 0 && board[i][j][k - 1] != board[i][j][k] && board[i][j][k - 1] != ' ' ||
								i != 0 && board[i - 1][j][k] != board[i][j][k] && board[i - 1][j][k] != ' ' ||
								j != 0 && board[i][j - 1][k] != board[i][j][k] && board[i][j - 1][k] != ' ')
							{
								adjCheck = 1;
								return -1;
							}
						}
					}
				}
			}
		}
		return 1;
	}


}
