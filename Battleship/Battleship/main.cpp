#include "InputUtilities.h"
#include "Bonus.h"
#include "GameUtilities.h"
#include "NaivePlayer.h"
#include "FilePlayer.h"
#include "SmartPlayer.h"

using namespace std;


int main(int argc, char** argv)
{
    string dirPath;
    string atkPathA;
    string atkPathB;
    string boardPath;
    string* board = new string[ROW_SIZE];
    vector<pair<int,int>> MovesA;
    vector<pair<int,int>> MovesB;
    FilePlayer A;
    FilePlayer B;
    DWORD sleepTime = DEFAULT_SLEEP_TIME;
    bool playWithGraphics = true;
    bool gotDirPath = false;
    char **boardA = new char *[ROW_SIZE];
    char **boardB = new char *[ROW_SIZE];
    for (int i = 0; i < ROW_SIZE; ++i)
    {
        boardA[i] = new char[COL_SIZE];
        boardB[i] = new char[COL_SIZE];
    }

    //processing program arguments
    if (argc >= 2)
    { // we accept the arguments in any order, and we assume that if a folder path is given it is the first argument
        for (int i = 1; i < argc; ++i)
        {
            if (!strcmp(argv[i], PARAM_QUIET))
            {
                playWithGraphics = false;
            }
            else if (!strcmp(argv[i], PARAM_DELAY))
            {
                if (i+1 < argc )
                {
                    char *p;
                    long delay = strtol(argv[i+1], &p, 10);
                    if (!*p)
                    {
                        sleepTime = (DWORD)delay;
                    }
                    // if there's no integer after PARAM_DELAY - we ignore this parameter...
                }
            }
            else if (i == 1)
            { // we assume that if there's a folder path it is the first argument
                dirPath = argv[1];
                if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
                {
                    return EXIT_FAILURE;
                }
                boardPath = dirPath + "/" + boardPath;
                atkPathA = dirPath + "/" + atkPathA;
                atkPathB = dirPath + "/" + atkPathB;
                gotDirPath = true;
            }
        }
    }
    if (!gotDirPath) // directory path given
    {
        dirPath = getDirPath();
        if (dirPath == BAD_STRING) //error occurred in getDirPath()
        {
            perror("Error");
            return EXIT_FAILURE;
        }
        if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
        {
            return EXIT_FAILURE;
        }
    }

    // setting up the main board
    initBoard(boardPath, board);
    // checking board validity
    if (checkBoardValidity(board) < 0)
    {
        return EXIT_FAILURE;
    }
    //setting up individual boards
    initIndividualBoards(board,boardA,boardB);
    // setting up attack vectors
    initAttack(atkPathA, MovesA);
    initAttack(atkPathB, MovesB);


    //now we pass the individual boards, attack vectors and ship liststo the players
    A.setBoard(0,(const char **)boardA, ROW_SIZE, COL_SIZE);
    A.initShipsList();
    A.setMoves(MovesA);
    B.setBoard(1,(const char **)boardB, ROW_SIZE, COL_SIZE);
    B.initShipsList();
    B.setMoves(MovesB);

    // Let the game begin!!!
    int attackerNum = 0;
	int defenderNum = 1;
    int sinkScore = 0;
    int scores[2] = {0}; // index 0 = A, index 1 = B
	FilePlayer *pPlayers[2] = { &A, &B };
    char hitChar;
    AttackResult attackResult;
    string attackerName = "A";

    if (playWithGraphics)
    {
		printOpeningMessage();
		Sleep(3*DEFAULT_SLEEP_TIME);
        // print the initial game board
        printBoard(board);
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
            changeCurrentPlayer(&attackerNum, &defenderNum);
            continue;
        }

        std::pair<int, int> currentMove = pPlayers[attackerNum]->attack();
        // should always pass this check - it's for debug purposes
        if (currentMove.first < 0 || currentMove.first >= ROW_SIZE ||
                currentMove.second < 0 || currentMove.second >= COL_SIZE )
        {
            cout << "Error: Invalid move from player " << attackerName << " - (" << currentMove.first << ","
                 << currentMove.second << ")" << endl;
            return EXIT_FAILURE;
        }
        hitChar = board[currentMove.first][currentMove.second];
		if (playWithGraphics)
		{
			clearLastLine();
			cout << attackerName << " shoots at (" << (currentMove.first + 1) << "," << (currentMove.second + 1) << ") - ";
			Sleep(sleepTime);
		}
        printSign(currentMove.first, currentMove.second, COLOR_RED, BOMB_SIGN, sleepTime, playWithGraphics);

		//cout << "char shot: $" << hitChar << "$" << endl;
        if (hitChar == WATER)
        {
            // Miss
			if (playWithGraphics)
			{
				cout << "MISS\r";
			}
            printSign(currentMove.first, currentMove.second, COLOR_DEFAULT_WHITE, WATER, sleepTime, playWithGraphics);
        }
        else // Hit xor Sink xor double hit xor hit a sunken ship
        {
            bool validAttack = pPlayers[(isupper(hitChar) ? 0 : 1)]->registerHit(currentMove, charToShipType(hitChar), attackResult);
            //notify players on attack results
            A.notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, attackResult);
            B.notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, attackResult);
            if(attackResult == AttackResult::Sink)
            {
                //Sink
                // calculate the score
                sinkScore = calculateSinkScore(hitChar);
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
					clearLastLine();
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
			printSign(currentMove.first, currentMove.second, (isupper(hitChar) ? COLOR_GREEN : COLOR_YELLOW), HIT_SIGN,
				sleepTime, playWithGraphics);
            // in case where there was a "real" hit (i.e a "living" tile got a hit) and it wasn't a self it,
            // the attacker gets anothen turn
            if(validAttack && !(isupper(hitChar) ^ attackerNum))
            {
                continue;
            }
        }
        //Change player
        attackerName = attackerNum ? "A" : "B";
        changeCurrentPlayer(&attackerNum, &defenderNum);
    }
	if (playWithGraphics)
	{
		clearLastLine();
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
    for (int i = 0; i < COL_SIZE; ++i)
    {
        delete[] boardA[i];
        delete[] boardB[i];
    }
    delete[] boardA;
    delete[] boardB;
    delete[] board;

    return EXIT_SUCCESS;
}





