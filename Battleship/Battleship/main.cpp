#include "Graphics.h"
#include "GameManagerUtilities.h"
#include "NaivePlayer.h"
#include "FilePlayer.h"
#include "SmartPlayer.h"
#include <iostream>

using namespace std;



int main(int argc, char** argv)
{
	string dirPath, boardPath, board[ROW_SIZE], dllPathA, dllPathB;
	auto sleepTime = Graphics::DEFAULT_GRAPHICS_DELAY;
	auto playWithGraphics = true, retVal = true;
	//processing program arguments
	GameManagerUtilities::processInputArguments(argc, argv, playWithGraphics, sleepTime, dirPath);
	if (GameManagerUtilities::getBoardPath(dirPath, boardPath) < 0)
	{
		return EXIT_FAILURE;
	}
	auto boardA = GameUtilities::allocateBoard(ROW_SIZE, COL_SIZE);
	auto boardB = GameUtilities::allocateBoard(ROW_SIZE, COL_SIZE);

	if (GameManagerUtilities::initGameBoards(boardPath, board, boardA, boardB) < 0)
	{
		return EXIT_FAILURE;
	}

	IBattleshipGameAlgo *A = new FilePlayer;
	IBattleshipGameAlgo *B = new FilePlayer;
	IBattleshipGameAlgo *pPlayers[2] = { A, B };
	PlayerAttributes playerAttributesArr[2];

	retVal = GameManagerUtilities::initPlayer(A, 0, const_cast<const char **>(boardA), dirPath, playerAttributesArr);
	if (!GameManagerUtilities::initPlayer(B, 1, const_cast<const char **>(boardB), dirPath, playerAttributesArr) || !retVal)
	{
		return EXIT_FAILURE;
	}


	// Let the game begin!!!
	auto attackerNum = 0, defenderNum = 1; // index 0 = A, index 1 = B
	int sinkScore; 
	char hitChar;
	AttackResult attackResult;
	string attackerName = "A";

	if (playWithGraphics)
	{
		Graphics::printOpeningMessage();
		Sleep(3*Graphics::DEFAULT_GRAPHICS_DELAY);
		// print the initial game board
		Graphics::printBoard(board, ROW_SIZE, COL_SIZE);
		Sleep(sleepTime);
	}
	//The game goes on until one of the players has no more ships or both ran out of moves.
	while (playerAttributesArr[0].shipsCount > 0 && playerAttributesArr[1].shipsCount > 0 && 
			(playerAttributesArr[0].hasMoves || playerAttributesArr[1].hasMoves))
	{
		//Skip if current player is out of moves.
		if (!playerAttributesArr[attackerNum].hasMoves)
		{
			GameManagerUtilities::changeCurrentPlayer(attackerNum, defenderNum, attackerName);
			continue;
		}

		auto currentMove = pPlayers[attackerNum]->attack();
		if (!GameUtilities::isLegalMove(currentMove, ROW_SIZE, COL_SIZE))
		{
			if (currentMove.first == -1 && currentMove.second == -1) // an exception - means no more moves
			{
				// attacker has ran out of moves
				playerAttributesArr[attackerNum].hasMoves = false;
				GameManagerUtilities::changeCurrentPlayer(attackerNum, defenderNum, attackerName);
				continue;
			}
			
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
			A->notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, AttackResult::Miss);
			B->notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, AttackResult::Miss);
		}
		else // Hit xor Sink xor double hit xor hit a sunken ship
		{
			auto validAttack = GameManagerUtilities::registerHit(playerAttributesArr[(isupper(hitChar) ? 0 : 1)], 
								currentMove,GameUtilities::charToShipType(hitChar), attackResult);
			//notify players on attack results
			A->notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, attackResult);
			B->notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, attackResult);
			if(attackResult == AttackResult::Sink)
			{
				//Sink
				// calculate the score
				sinkScore = GameManagerUtilities::calculateSinkScore(hitChar);
				if (sinkScore == -1) // for debug - should not get here
				{
					cout << "Error: Unexpected char on board: " << hitChar << endl;
					return EXIT_FAILURE;
				}
				// if hitChar is an UPPERCASE char - than A was hit and B gets the points (and vice versa)
				playerAttributesArr[(isupper(hitChar) ? 1 : 0)].score += sinkScore;
				if (playWithGraphics)
				{
					cout << (!isupper(hitChar) == attackerNum ? "SELF-SINK" : "SINK") << "\r";
					Sleep(sleepTime);
					Graphics::clearLastLine();
					cout << "CURRENT SCORE: A-" << playerAttributesArr[0].score << ", B-" << playerAttributesArr[1].score << "\r";
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
		GameManagerUtilities::changeCurrentPlayer(attackerNum, defenderNum,attackerName);
	}
	GameManagerUtilities::printGameResults(playerAttributesArr, playWithGraphics);

	// delete all local boards
	GameUtilities::deleteBoard(boardA, ROW_SIZE);
	GameUtilities::deleteBoard(boardB, ROW_SIZE);
	delete A;
	delete B;

	return EXIT_SUCCESS;
}