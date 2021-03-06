﻿#include "GameManagerUtilities.h"
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
#include "Graphics.h"

namespace GameManagerUtilities
{
	int initGameBoardsAndPaths(int argc, char **argv, string(&board)[ROW_SIZE], char **boardA, char* *boardB,
		string& dirPath, string& dllPathA, string& dllPathB)
	{
		auto errorOccurred = false;
		string boardPath;
		// process program arguments
		if (processInputArguments(argc, argv, dirPath) < 0)
		{
			return -1;
		}
		// validate game configuration files
		if (!isValidPath(dirPath))
		{
			cout << "Wrong Path: " << dirPath << endl;
			return -1;
		}
		// init boards
		if (getPathByType(boardPath, dirPath, BOARD_FILE_SUFFIX) < 0 
			|| initGameBoards(boardPath, board, boardA, boardB) < 0)
		{
			errorOccurred = true;
		}
		if (getPathByType(dllPathA, dirPath, LIB_FILE_SUFFIX, eFileType::DLL_FILE, 0) < 0 || 
			getPathByType(dllPathB, dirPath, LIB_FILE_SUFFIX, eFileType::DLL_FILE, 1) < 0)
		{
			errorOccurred = true;
		}
		return errorOccurred ? -1 : 0;
	}

	int GameManagerUtilities::processInputArguments(int argc, char** argv, string& dirPath)
	{
		auto gotDirPath = false;
		if (argc >= 2)
		{ // we accept the arguments in any order, and we assume that if a folder path is given it is the first argument
			for (auto i = 1; i < argc; ++i)
			{
				if (!strcmp(argv[i], PARAM_QUIET.c_str()))
				{
					Graphics::playWithGraphics = false;
				}
				else if (!strcmp(argv[i], PARAM_DELAY.c_str()))
				{
					if (i + 1 < argc)
					{
						char *p;
						auto delay = strtol(argv[i + 1], &p, 10);
						if (!*p)
						{
							Graphics::sleepTime = static_cast<DWORD>(delay);
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
		// convert the given directory path to full path
		return convertToFullPath(dirPath);
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
			return -1000;
		}
	}

	void GameManagerUtilities::changeCurrentPlayer(int& attackerNum, int& defenderNum, string& attackerName)
	{
		attackerName = attackerNum ? "A" : "B";
		// switch attacker and defender
		attackerNum ^= 1;
		defenderNum ^= 1;
	}

	void GameManagerUtilities::printGameResults(PlayerAttributes playerAttributesArr[])
	{
		if (Graphics::playWithGraphics)
		{
			Graphics::clearLastLine();
			cout << '\a';
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
		set<char> charSet{ WATER, BOAT,MISSLE_SHIP, SUBMARINE, DESTROYER, char(tolower(BOAT)),
							char(tolower(MISSLE_SHIP)), char(tolower(SUBMARINE)), char(tolower(DESTROYER)) };
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
		if (horL > 1 && verL > 1 || horL != size && verL != size)
		{
			return -1;
		}
		return 1;
	}

	bool GameManagerUtilities::registerHit(PlayerAttributes& playerAttributes, pair<int, int> coords, eShipType shipType, AttackResult& res)
	{
		auto i = 0;
		auto validAttack = false;
		for (; i < DEFAULT_SHIPS_COUNT; i++)
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

	bool GameManagerUtilities::initPlayer(IBattleshipGameAlgo*& pPlayer, int playerNum, const char** board,
		const string dirPath, const string dllPath, PlayerAttributes playerAttributesArr[], DLLManager& dllMngr)
	{
		pPlayer = dllMngr.loadAlgo(dllPath);
		if (pPlayer == nullptr)
		{
			return false;
		}
		pPlayer->setBoard(playerNum, board, ROW_SIZE, COL_SIZE);
		initPlayersAttributes(playerAttributesArr[playerNum], board);
		if (!pPlayer->init(dirPath))
		{
			cout << "Algorithm initialization failed for dll: " << dllPath << endl;
			return false;
		}
		return true;
	}

	int GameManagerUtilities::printBoardErrors(bitset<4>& errShipsA, bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck)
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

	void GameManagerUtilities::printWrongSizeOrShapeError(bitset<4>& errShips, int& ret, const string player)
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

	void GameManagerUtilities::printBadShipsCountErrror(int shipCount, int& ret, const string player)
	{
		if (shipCount != DEFAULT_SHIPS_COUNT)
		{
			string filler = shipCount > DEFAULT_SHIPS_COUNT ? "many" : "few";
			cout << "Too " << filler << " ships for player " << player << endl;
			ret = -1;
		}
	}

	int GameManagerUtilities::checkBoardValidity(string* board)
	{
		auto shipCountA = 0, shipCountB = 0, isShipA = 0, isShipB = 0, adjCheck = 0;
		map<char, int> shipsA = { { BOAT,1 },{ MISSLE_SHIP,2 },{ SUBMARINE,3 },{ DESTROYER,4 } };
		map<char, int> shipsB = { { char(tolower(BOAT)),1 },{ char(tolower(MISSLE_SHIP)),2 },
								{ char(tolower(SUBMARINE)),3 },{ char(tolower(DESTROYER)),4 } };
		bitset<4> errShipsA, errShipsB;// error flags for ship misshapes

		for (auto i = 0; i < ROW_SIZE; i++)
		{
			for (auto j = 0; j < COL_SIZE; j++)
			{
				if (board[i][j] != ' ')
				{
					isShipA = shipsA[board[i][j]] != 0;       // 1 if its a ship, otherwise 0
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

	int GameManagerUtilities::convertToFullPath(string& dirPath)
	{
		if (dirPath == BAD_STRING) //error occurred in getDirPath()
		{
			perror("Error");
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

	string GameManagerUtilities::getDirPath()
	{
		char *buff = nullptr;
		buff = _getcwd(buff, MAX_PATH_LEN);
		if (!buff)
		{
			return BAD_STRING; //signs the string is bad
		}
		string temp(buff);
		delete buff; // memory was allocated by _getcwd
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
		playerAttributes.shipsCount = DEFAULT_SHIPS_COUNT;
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
		return Ship(size, charToShipType(c), coordinates);
	}


	int GameManagerUtilities::playTheGame(IBattleshipGameAlgo* A, IBattleshipGameAlgo* B, PlayerAttributes playerAttributesArr[], const string* board)
	{
		auto attackerNum = 0, defenderNum = 1; // index 0 = A, index 1 = B
		string attackerName = "A";
		IBattleshipGameAlgo *pPlayers[] = { A , B };

		Graphics::showConsoleCursor(false); //hide the console cursor

		Graphics::printStartOfGame(board, ROW_SIZE, COL_SIZE);

		//The game goes on until one of the players has no more ships or both ran out of moves.
		while (playerAttributesArr[0].shipsCount > 0 && playerAttributesArr[1].shipsCount > 0 &&
			(playerAttributesArr[0].hasMoves || playerAttributesArr[1].hasMoves))
		{
			//Skip if current player is out of moves.
			if (!playerAttributesArr[attackerNum].hasMoves)
			{
				changeCurrentPlayer(attackerNum, defenderNum, attackerName);
				continue;
			}

			auto currentMove = pPlayers[attackerNum]->attack();
			if (!isLegalMove(currentMove, ROW_SIZE, COL_SIZE))
			{
				if (currentMove.first == -1 && currentMove.second == -1) // an exception - means no more moves
				{
					playerAttributesArr[attackerNum].hasMoves = false; // attacker has ran out of moves
				}
				// we switch player and continue - even if the move is invalid (i.e we ignore invalid moves)
				changeCurrentPlayer(attackerNum, defenderNum, attackerName);
				continue;

			}
			handleMove(board, currentMove, attackerNum, defenderNum, attackerName, A, B, playerAttributesArr);
		}
		printGameResults(playerAttributesArr);
		delete A;
		delete B;
		Graphics::showConsoleCursor(true); //show the console cursor again
		return EXIT_SUCCESS;
	}


	void GameManagerUtilities::handleMove(const string *board, pair<int, int> move, int &attackerNum, int &defenderNum, string &attackerName, 
										IBattleshipGameAlgo *A, IBattleshipGameAlgo *B, PlayerAttributes playerAttributesArr[])
	{
		bool validAttack;
		//remember moves are from 1 to ROW/COL SIZE while the board is from 0 to ROW/COL SIZE -1
		// hence we need to give a (-1) offset to the move coordinates
		auto hitChar = board[move.first - 1][move.second - 1];
		if (Graphics::playWithGraphics)
		{
			Graphics::clearLastLine();
			cout << attackerName << " shoots at (" << move.first << "," << move.second << ") - ";
			Sleep(Graphics::sleepTime);
		}
		Graphics::printSign(move.first, move.second, COLOR_RED, Graphics::BOMB_SIGN);
		if (hitChar == WATER)
		{
			// Miss
			handleMiss(move, A, B, attackerNum);
		}
		else // Hit xor Sink xor double hit xor hit a sunken ship
		{
			handleHitOrSink(move, validAttack, A, B, hitChar, attackerNum, playerAttributesArr);
			// in case where there was a "real" hit (i.e a "living" tile got a hit) and it wasn't a self it, the attacker gets another turn
			if (validAttack && !(isupper(hitChar) ^ attackerNum))
			{
				return;
			}
		}
		//Change player
		changeCurrentPlayer(attackerNum, defenderNum, attackerName);
	}

	void GameManagerUtilities::handleMiss(pair<int,int> move, IBattleshipGameAlgo *A, IBattleshipGameAlgo *B ,int attackerNum)
	{
		if (Graphics::playWithGraphics)
		{
			cout << "MISS\r";
		}
		Graphics::printSign(move.first, move.second, COLOR_DEFAULT_WHITE, WATER);
		A->notifyOnAttackResult(attackerNum, move.first, move.second, AttackResult::Miss);
		B->notifyOnAttackResult(attackerNum, move.first, move.second, AttackResult::Miss);
		
	}

	void GameManagerUtilities::handleHitOrSink(pair<int,int> move, bool &validAttack, IBattleshipGameAlgo *A, IBattleshipGameAlgo *B,
	                                           char hitChar, int attackerNum, PlayerAttributes playerAttributesArr[])
	{
		AttackResult attackResult;
		validAttack = registerHit(playerAttributesArr[(isupper(hitChar) ? 0 : 1)], move, charToShipType(hitChar), attackResult);
		//notify players on attack results
		A->notifyOnAttackResult(attackerNum, move.first, move.second, attackResult);
		B->notifyOnAttackResult(attackerNum, move.first, move.second, attackResult);
		if (attackResult == AttackResult::Sink)
		{
			// if hitChar is an UPPERCASE char - than A was hit and B gets the points (and vice versa)
			playerAttributesArr[(isupper(hitChar) ? 1 : 0)].score += calculateSinkScore(hitChar);
			Graphics::printSink(hitChar, attackerNum, playerAttributesArr[0].score, playerAttributesArr[1].score);
		}
		else
		{
			Graphics::printHit(validAttack, attackResult == AttackResult::Hit, hitChar, attackerNum);
		}
		Graphics::printSign(move.first, move.second, isupper(hitChar) ? COLOR_GREEN : COLOR_YELLOW, Graphics::HIT_SIGN);
		
	}
}