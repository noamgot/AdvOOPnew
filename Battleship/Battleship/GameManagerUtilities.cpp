#include "GameManagerUtilities.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <bitset>
#include <fstream>
#include <direct.h>
#include <set>
#include <map>
#include <stdlib.h>
#include <Windows.h>
#include "AbstractPlayer.h"
#include "Graphics.h"


using namespace std;

// initialize static consts
const string GameManagerUtilities::PARAM_QUIET("-quiet");
const string GameManagerUtilities::PARAM_DELAY("-delay");
const string GameManagerUtilities::BOARD_FILE_SUFFIX(".sboard");
const string GameManagerUtilities::BAD_STRING("!@#"); // for getDirPath
const int GameManagerUtilities::MAX_PATH_LEN = 1024;

void GameManagerUtilities::processInputArguments(int argc, char** argv, bool& playWithGraphics, int& sleepTime, string& dirPath)
{
	auto gotDirPath = false;
	if (argc >= 2)
	{ // we accept the arguments in any order, and we assume that if a folder path is given it is the first argument
		for (auto i = 1; i < argc; ++i)
		{
			if (!strcmp(argv[i], PARAM_QUIET.c_str()))
			{
				playWithGraphics = false;
			}
			else if (!strcmp(argv[i], PARAM_DELAY.c_str()))
			{
				if (i + 1 < argc)
				{
					char *p;
					auto delay = strtol(argv[i + 1], &p, 10);
					if (!*p)
					{
						sleepTime = static_cast<DWORD>(delay);
					}
					// if there's no integer after PARAM_DELAY - we ignore this parameter...
				}
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
}


int GameManagerUtilities::calculateSinkScore(char c)
{
	switch (toupper(c))
	{
	case BOAT:
		return BOAT_SCORE;
	case MISSLE_SHIP:
		return MISSLE_SHIP_SCORE;
	case SUBMARINE:
		return SUBMARINE_SCORE;
	case DESTROYER:
		return DESTROYER_SCORE;
	default:
		return -1;
	}
}

void GameManagerUtilities::changeCurrentPlayer(int& attackerNum, int& defenderNum)
{
	attackerNum = attackerNum ? 0 : 1;
	defenderNum = defenderNum ? 0 : 1;
}


void GameManagerUtilities::printGameResults(PlayerAttributes playerAttributesArr[], bool playWithGraphics)
{
	if (playWithGraphics)
	{
		Graphics::clearLastLine();
	}
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


int GameManagerUtilities::initGameBoards(const string boardPath, string board[], char** boardA, char** boardB)
{
	ifstream boardFile(boardPath);
	if (!boardFile.is_open())
	{
		cout << "Error: opening board file failed" << endl;
		return -1;
	}
	char chars[9] = { ' ','B','P','M','D','b','p','m','d' };
	set<char> charSet;
	charSet.insert(chars, chars + 9);

	for (auto i = 0; i < ROW_SIZE; i++)
	{
		getline(boardFile, board[i]);
		if (board[i].size() > 0 && board[i].back() == '\r')
		{
			board[i].back() = ' '; // handles '\r'
		}
		if (board[i].length() > COL_SIZE)
		{                     // ignores extra characters in line
			board[i].erase(COL_SIZE, board[i].length() - COL_SIZE);
		}
		else if (board[i].length() < COL_SIZE)
		{                // appends extra '_' in case of a short line
			board[i].append(COL_SIZE - board[i].length(), ' ');
		}
		if (boardFile.eof() && i < ROW_SIZE - 1)
		{                // inserts extra rows of '_' in case of missing lines
			i++;
			for (; i < ROW_SIZE; i++)
			{
				board[i] = string(COL_SIZE, ' ');
			}
		}
	}
	for (auto i = 0; i < ROW_SIZE; i++)
	{                        // convert non-valid characters to spaces
		for (auto j = 0; j < COL_SIZE; j++)
		{
			if (charSet.find(board[i][j]) == charSet.end()) board[i][j] = ' ';
		}
	}
	boardFile.close();
	// validate board
	if (checkBoardValidity(board) < 0)
	{
		return -1;
	}
	initIndividualBoards(board, boardA, boardB);
	return 0;
}

void GameManagerUtilities::initIndividualBoards(string *board, char **boardA, char **boardB)
{
	char c;
	for (auto i = 0; i < ROW_SIZE; ++i)
	{
		for (auto j = 0; j < COL_SIZE; ++j)
		{
			c = board[i][j];
			if (isalpha(c)) //part of a ship
			{
				if (isupper(c)) // a ship of A
				{
					boardA[i][j] = c;
					boardB[i][j] = WATER;
				}
				else // a ship of B
				{
					boardA[i][j] = WATER;
					boardB[i][j] = c;
				}
			}
			else // a space - update both boards
			{
				boardA[i][j] = WATER;
				boardB[i][j] = WATER;
			}
		}
	}
}

int GameManagerUtilities::checkShape(string* board, const int size, int i, int j)
{
	auto verL = 1, horL = 1;
	// run horizontally, check above and below
	while (j + horL < COL_SIZE && board[i][j] == board[i][j + horL])
	{
		if (i + 1 < ROW_SIZE && board[i][j] == board[i + 1][j + horL] ||
			i - 1 >= 0 && board[i][j] == board[i - 1][j + horL])
		{
			return -1;
		}
		horL++;
	}
	// run vertically, check right and left
	while (i + verL < ROW_SIZE && board[i][j] == board[i + verL][j])
	{
		if (j + 1 < COL_SIZE && board[i][j] == board[i + verL][j + 1] ||
			j - 1 >= 0 && board[i][j] == board[i + verL][j - 1])
		{
			return -1;
		}
		verL++;
	}
	// check for misshape in size
	if ((horL > 1 && verL > 1) || (horL != size && verL != size))
	{
		return -1;
	}
	return 1;
}

bool GameManagerUtilities::registerHit(PlayerAttributes& playerAttributes, pair<int, int> coords, eShipType shipType, AttackResult& res)
{
	auto i = 0;
	auto validAttack = false;
	for (; i < GameUtilities::DEFAULT_SHIPS_COUNT; i++)
	{
		if (playerAttributes.shipList[i].getType() == shipType)
		{
			//Make sure this coordinate belongs to this ship
			if (playerAttributes.shipList[i].getCoordinates().count(coords) == 1)
			{
				validAttack = playerAttributes.shipList[i].handleHit(coords, res);
				if (res == AttackResult::Sink)
				{
					playerAttributes.shipsCount--;
				}
				break;
			}
		}
	}
	return validAttack;
}

bool GameManagerUtilities::initPlayer(IBattleshipGameAlgo* pPlayer, int playerNum, const char** board,
									  const string dirPath, PlayerAttributes playerAttributesArr[])
{
	pPlayer->setBoard(playerNum, board, ROW_SIZE, COL_SIZE);
	initPlayersAttributes(playerAttributesArr[playerNum], board);
	return pPlayer->init(dirPath);
}

int GameManagerUtilities::printBoardErrors(bitset<4>& errShipsA, bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck)
{
	auto ret = 0;
	string filler;
	char shipMap[4] = { BOAT,MISSLE_SHIP,SUBMARINE,DESTROYER };
	// Print possible errors
	for (auto i = 0; i < 4; i++)
	{
		if (errShipsA[i])
		{
			cout << "Wrong size or shape for ship " << shipMap[i] << " for player A" << endl;
			ret = -1;
		}
	}
	for (auto i = 0; i < 4; i++)
	{
		if (errShipsB[i]) {
			cout << "Wrong size or shape for ship " << static_cast<char>(tolower(shipMap[i])) << " for player B" << endl;
			ret = -1;
		}
	}
	if (shipCountA != GameUtilities::DEFAULT_SHIPS_COUNT)
	{
		filler = (shipCountA > GameUtilities::DEFAULT_SHIPS_COUNT) ? "many" : "few";
		cout << "Too " << filler << " ships for Player A" << endl;
		ret = -1;
	}
	if (shipCountB != GameUtilities::DEFAULT_SHIPS_COUNT)
	{
		filler = (shipCountB > GameUtilities::DEFAULT_SHIPS_COUNT) ? "many" : "few";
		cout << "Too " << filler << " ships for Player B" << endl;
		ret = -1;
	}
	if (adjCheck)
	{
		cout << "Adjacent Ships on Board" << endl;
		ret = -1;
	}
	return ret;
}

int GameManagerUtilities::checkBoardValidity(string* board)
{
	auto shipCountA = 0, shipCountB = 0, isShipA = 0, isShipB = 0, adjCheck = 0;
	map<char, int> shipsA = { { 'B',1 },{ 'P',2 },{ 'M',3 },{ 'D',4 } };
	map<char, int> shipsB = { { 'b',1 },{ 'p',2 },{ 'm',3 },{ 'd',4 } };
	bitset<4> errShipsA, errShipsB;// error flags for ship misshapes

	for (auto i = 0; i < ROW_SIZE; i++)
	{
		for (auto j = 0; j < COL_SIZE; j++)
		{
			if (board[i][j] != ' ')
			{
				isShipA = shipsA[board[i][j]] != 0;       // 1 if its A ship, otherwise 0
				isShipB = shipsB[board[i][j]] != 0;
				if (isShipA || isShipB)
				{
					// check if its new
					if (!(i != 0 && board[i - 1][j] == board[i][j] ||
						j != 0 && board[i][j - 1] == board[i][j]))
					{
						// check for misshape
						if (checkShape(board, shipsB[tolower(board[i][j])], i, j) < 0)
						{
							if (isShipA)
							{
								errShipsA[shipsA[board[i][j]] - 1] = 1;
							}
							if (isShipB)
							{
								errShipsB[shipsB[board[i][j]] - 1] = 1;
							}
						}
						else
						{
							shipCountA += isShipA;
							shipCountB += isShipB;
						}
					}
					// Check if any adjacent ships exist
					if (j != 0 && board[i][j - 1] != board[i][j] && board[i][j - 1] != ' ' ||
						i != 0 && board[i - 1][j] != board[i][j] && board[i - 1][j] != ' ')
					{
						adjCheck = 1;
					}
				}
			}
		}
	}

	return printBoardErrors(errShipsA, errShipsB, shipCountA, shipCountB, adjCheck);
}

int GameManagerUtilities::getBoardPath(string& dirPath, string& boardPath)
{
	if (dirPath == BAD_STRING) //error occurred in getDirPath()
	{
		perror("Error");
		return -1;
	}
	// get the full path of the directory
	auto fullPath = _fullpath(nullptr, dirPath.c_str(), MAX_PATH_LEN);
	// copy the full path to dirPath and delete fullPath
	dirPath = fullPath;
	delete fullPath;
	// clear "\\" if added by _fullpath()
	if (dirPath.back() == '\\')
	{
		dirPath.pop_back();
	}
	//TODO - add DLL search
	if (!GameUtilities::isValidPath(dirPath))
	{
		cout << "Wrong Path: " << dirPath << endl;
		return -1;
	}
	auto fileNotFound = true;
	if (GameUtilities::findFileBySuffix(boardPath, dirPath, BOARD_FILE_SUFFIX, fileNotFound, 0) < 0)
	{
		if (fileNotFound)
		{
			cout << "Missing board file (*" << BOARD_FILE_SUFFIX << ") looking in path: " << dirPath << endl;
		}
		return -1;
	}
	// convert boardPath to its full path
	boardPath = dirPath + "\\" + boardPath;
	return 0;
}

string GameManagerUtilities::getDirPath()
{
	char *buff = nullptr;
	buff = _getcwd(buff, MAX_PATH_LEN);
	if (!buff)
	{
		return BAD_STRING; //signs the string is bad
	}
	string temp(buff);
	delete buff;
	return temp;
}

void GameManagerUtilities::initPlayerShipsList(const char** board, int numOfRows, int numOfCols, PlayerAttributes& playerAttributes)
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

void GameManagerUtilities::initPlayersAttributes(PlayerAttributes& playerAttributes, const char** playerBoard)
{
	playerAttributes.hasMoves = true;
	playerAttributes.score = 0;
	playerAttributes.shipsCount = GameUtilities::DEFAULT_SHIPS_COUNT;
	initPlayerShipsList(playerBoard, ROW_SIZE, COL_SIZE, playerAttributes);
}

Ship GameManagerUtilities::handleShipDiscovery(int iOrig, int jOrig, int numOfRows, int numOfCols, const char** board)
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
	return Ship(size, GameUtilities::charToShipType(c), coordinates);
}
