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
#include "MyBoardData.h"

using namespace std;
using namespace CommonUtilities;

namespace GameUtilities
{

	int GameUtilities::printBoardErrors(bitset<4>& errShipsA, bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck)
	{
		auto ret = 0;
		// Print possible errors
		printWrongSizeOrShapeError(errShipsA, ret, "A");
		printWrongSizeOrShapeError(errShipsB, ret, "B");
		printBadShipsCountError(shipCountA, shipCountB);
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

	void GameUtilities::printBadShipsCountError(int shipCountA, int shipCountB)
	{
		if (shipCountA != shipCountB)
		{
			//todo - LOG an unfair board
		}
	}
	
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

}
