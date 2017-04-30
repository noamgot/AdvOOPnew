#include "InputUtilities.h"
#include "Graphics.h"
#include "GameUtilities.h"
#include "NaivePlayer.h"
#include "FilePlayer.h"
#include "SmartPlayer.h"

using namespace std;


int main(int argc, char** argv)
{
	string dirPath, atkPathA, atkPathB, boardPath, dllPathA, dllPathB;
	auto board = new string[ROW_SIZE];
	vector<pair<int, int>> MovesA, MovesB;
	auto sleepTime = Graphics::DEFAULT_GRAPHICS_DELAY;
	auto playWithGraphics = true;
	FilePlayer A, B;

	//processing program arguments
	if (GameUtilities::processInputArguments(argc, argv, playWithGraphics, sleepTime, 
												dirPath, boardPath, atkPathA, atkPathB) < 0)
	{
		return EXIT_FAILURE;
	}

	auto boardA = GameUtilities::allocateBoard(ROW_SIZE, COL_SIZE);
	auto boardB = GameUtilities::allocateBoard(ROW_SIZE, COL_SIZE);

	if (GameUtilities::initGameBoards(boardPath, board, boardA, boardB) < 0)
	{
		return EXIT_FAILURE;
	}

	
	/*
	// setting up the main board
	initBoard(boardPath, board);
	// checking board validity
	if (checkBoardValidity(board) < 0)
	{
		return EXIT_FAILURE;
	}
	//setting up individual boards
	
	initIndividualBoards(board,boardA,boardB);
	*/
	// setting up attack vectors
	//TODO @Ben - change to queues instead vectors
	initAttack(atkPathA, MovesA);
	initAttack(atkPathB, MovesB);

	//now we pass the individual boards, attack vectors and ship lists to the players
	A.setBoard(0,const_cast<const char **>(boardA), ROW_SIZE, COL_SIZE);
	A.init(dirPath);
	B.setBoard(1,const_cast<const char **>(boardB), ROW_SIZE, COL_SIZE);
	B.init(dirPath);
	B.setMoves(MovesB);

	// Let the game begin!!!
	auto attackerNum = 0, defenderNum = 1;
	int sinkScore;
	int scores[2] = {0}; // index 0 = A, index 1 = B
	FilePlayer *pPlayers[2] = { &A, &B };
	char hitChar;
	AttackResult attackResult;
	string attackerName = "A";

	if (playWithGraphics)
	{
		Graphics::printOpeningMessage();
		Sleep(3*Graphics::DEFAULT_GRAPHICS_DELAY);
		// print the initial game board
		Graphics::printBoard(board);
		Sleep(sleepTime);
	}
	//The game goes on until one of the players has no more ships or both ran out of moves.
	while (pPlayers[0]->hasShips() && pPlayers[1]->hasShips() && (pPlayers[0]->hasMoves() || pPlayers[1]->hasMoves()))
	{
		//Skip if current player is out of moves.
		if (!pPlayers[attackerNum]->hasMoves())
		{
			//cout << "Player " << attackerName << " has ran out of moves - SWITCHING PLAYER" << endl;
			attackerName = attackerNum ? "A" : "B";
			GameUtilities::changeCurrentPlayer(&attackerNum, &defenderNum);
			continue;
		}

		auto currentMove = pPlayers[attackerNum]->attack();
		// should always pass this check - it's for debug purposes
		if (currentMove.first < 1 || currentMove.first > ROW_SIZE ||
				currentMove.second < 1 || currentMove.second > COL_SIZE )
		{
			cout << "Error: Invalid move from player " << attackerName << " - (" << currentMove.first << ","
				 << currentMove.second << ")" << endl;
			return EXIT_FAILURE;
		}
		//remember moves are from 1 to ROW/COL SIZE while the board is from 0 to ROW/COL SIZE -1
		// hence we need to give a (-1) offset to the move coordinates
		hitChar = board[currentMove.first-1][currentMove.second-1];
		if (playWithGraphics)
		{
			Graphics::clearLastLine();
			cout << attackerName << " shoots at (" << (currentMove.first) << "," << (currentMove.second) << ") - ";
			Sleep(sleepTime);
		}
		Graphics::printSign(currentMove.first, currentMove.second, COLOR_RED, Graphics::BOMB_SIGN, sleepTime, playWithGraphics);

		if (hitChar == WATER)
		{
			// Miss
			if (playWithGraphics)
			{
				cout << "MISS\r";
			}
			Graphics::printSign(currentMove.first, currentMove.second, COLOR_DEFAULT_WHITE, WATER, sleepTime, playWithGraphics);
		}
		else // Hit xor Sink xor double hit xor hit a sunken ship
		{
			auto validAttack = pPlayers[(isupper(hitChar) ? 0 : 1)]->registerHit(currentMove, GameUtilities::charToShipType(hitChar), attackResult);
			//notify players on attack results
			A.notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, attackResult);
			B.notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, attackResult);
			if(attackResult == AttackResult::Sink)
			{
				//Sink
				// calculate the score
				sinkScore = GameUtilities::calculateSinkScore(hitChar);
				if (sinkScore == -1) // for debug - should not get here
				{
					cout << "Error: Unexpected char on board: " << hitChar << endl;
					return EXIT_FAILURE;
				}
				// if hitChar is an UPPERCASE char - than A was hit and B gets the points (and vice versa)
				scores[(isupper(hitChar) ? 1 : 0)] += sinkScore;
				if (playWithGraphics)
				{
					cout << (!isupper(hitChar) == attackerNum ? "SELF-SINK" : "SINK") << "\r";
					Sleep(sleepTime);
					Graphics::clearLastLine();
					cout << "CURRENT SCORE: A-" << scores[0] << ", B-" << scores[1] << "\r";
					Sleep(sleepTime);
				}
			}
			else
			{
				if (playWithGraphics)
				{
					if (validAttack && attackResult == AttackResult::Hit)
					{
						//Hit xor self hit
						cout << (!isupper(hitChar) == attackerNum ? "SELF-HIT" : "HIT") << "\r";
					}
					else
					{
						cout << "ALREADY HIT\r";
					}
				}
			}
			Graphics::printSign(currentMove.first, currentMove.second, (isupper(hitChar) ? COLOR_GREEN : COLOR_YELLOW), Graphics::HIT_SIGN,
				sleepTime, playWithGraphics);
			// in case where there was a "real" hit (i.e a "living" tile got a hit) and it wasn't a self it,
			// the attacker gets another turn
			if(validAttack && !(isupper(hitChar) ^ attackerNum))
			{
				continue;
			}
		}
		//Change player
		attackerName = attackerNum ? "A" : "B";
		GameUtilities::changeCurrentPlayer(&attackerNum, &defenderNum);
	}
	if (playWithGraphics)
	{
		Graphics::clearLastLine();
	}
	if(!pPlayers[0]->hasShips())
	{
		cout << "Player B won" << endl;
	}
	else if(!pPlayers[1]->hasShips())
	{
		cout << "Player A won" << endl;
	}
	cout << "Points:\nPlayer A:" << scores[0] << "\nPlayer B:" << scores[1] << endl;

	// delete all local boards
	GameUtilities::deleteBoard(boardA, ROW_SIZE);
	GameUtilities::deleteBoard(boardB, ROW_SIZE);
	delete[] board;

	return EXIT_SUCCESS;
}





