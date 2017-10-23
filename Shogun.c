/*
Author: Joe Ezaki
		Shogun
		Project 2
		CSc 180
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

// Minimax constants
#define EVAL_ENDING_COMP 999
#define EVAL_ENDING_HUM -999
#define MAX_DEPTH 10

// Board size constants
#define BOARD_LENGTH 8
#define BOARD_WIDTH 7
#define MAX_MOVES 20000

// Global variables
int gBoard[BOARD_LENGTH][BOARD_WIDTH] = {{0, 0, 0, -5, 0, 0, 0},
			 {-3, -3, -3, 0, -4, -4, -4},
			 {-2, -2, -2, 0, -1, -1, -1},
			 {0, 0, 0, 0, 0, 0, 0},
			 {0, 0, 0, 0, 0, 0, 0},
			 {1, 1, 1, 0, 2, 2, 2},
			 {4, 4, 4, 0, 3, 3, 3},
			 {0, 0, 0, 5, 0, 0, 0}}; // The board
int gMoves[MAX_MOVES]; // Array of possible moves
int gMIndex;
int humanKillerMove;
int compKillerMove;
int historyTable[8787];
int pruned;
int totalNodes;

// Function Definitions
void playGame();
void displayBoard();
void getMoves(bool compTurn);
bool gameOver();
int miniMax();
int minMiniMax(int depth, int max);
int maxMiniMax(int depth, int min);
int eval();
void sort(int start, int stop, bool compTurn);

int main()
{
	gMIndex = 0;
	playGame();
	exit(0);
}


void playGame()
{
	time_t start, stop;
	char moveString[5] = {'\0','\0','\0','\0','\0'};

	bool play = true;
	bool compTurn = false;
	bool legal = false;
	char input;
	int counter =0;
	printf("Do you want to go first (y/n)? ");
	if (scanf("%c", &input)!=1) printf("oh no!\n");;
	if (input == 'n')
		compTurn = true;
	displayBoard();
	while (play)
	{
		int move = -1;
        int oldLength, oldWidth, newLength, newWidth;
		getMoves(compTurn);
        if (gBoard[0]>0)
        {
			if (compTurn)
			{	
				start = time(NULL);
				move = miniMax();
				stop = time(NULL);
				printf("Total search time: %f\n", difftime(stop, start));
				moveString[0] = (char)((move/1000)+'A');
				moveString[1] = (char)((move%1000/100)+49);
				moveString[2] = (char)((move%100/10)+'A');
				moveString[3] = (char)((move%10)+49);
				printf("My move is: %s ", moveString);
				moveString[0] = (char)(abs((move/1000)-6)+'A');
				moveString[1] = (char)(abs((move%1000/100)-8) + 48);
				moveString[2] = (char)(abs((move%100/10)-6)+'A');
				moveString[3] = (char)(abs((move%10)-8)+48);
				printf("(%s)\n", moveString);
			}
			
			// Human player's turn
			else
			{
				legal = false;
				char codedMove[4];
				while(!legal)
				{
					int i;
					printf("Please choose one of these possible moves: \n");
					for (i = 0; i < gMIndex; i++)
					{
						move = gMoves[i];
						moveString[0] = (char)((move/1000)+'A');
						moveString[1] = (char)((move%1000/100)+49);
						moveString[2] = (char)((move%100/10)+'A');
						moveString[3] = (char)((move%10)+49);
						printf("%s ", moveString);
					}
					printf("\n");
					if (scanf("%s", codedMove)!=1) printf("Uh-oh\n");
					move = (1000*(int)(toupper(codedMove[0])-'A')) + (100*(int)(codedMove[1]-'1')) + (10*(int)(toupper(codedMove[2])-'A')) + (int)(codedMove[3]-'1');
					i = 0;
					for (i = 0; i < gMIndex; i++)
					{
						if (gMoves[i]==move)
						{
							legal = true;
							i = gMIndex;
						}
					}
					if (!legal)
						printf("That's not a legal move.  Try again.\n");
				}
                gMIndex = 0;
			} // End player's turn
			// Decyphering 4 digit move
			oldLength = (move%1000)/100;
			oldWidth = (move/1000);
			newLength = (move%10);
			newWidth = (move%100)/10;
			// Make the actual move
			gBoard[newLength][newWidth] = gBoard[oldLength][oldWidth];
			gBoard[oldLength][oldWidth] = 0;
			// It's an attack move if the piece in front of where the move lands is an enemy
			// Decrement the piece, increment the attack flag
			if (newLength < BOARD_LENGTH-1 && !compTurn && gBoard[newLength+1][newWidth] > 0 && gBoard[newLength+1][newWidth] != 5)
			{	
				gBoard[newLength+1][newWidth] = (int)(fmax(gBoard[newLength+1][newWidth]-2, 0));
			}
			// Computer's attack
			else if(compTurn && newLength > 0 && gBoard[newLength-1][newWidth] < 0 && gBoard[newLength-1][newWidth] != -5)
			{
				gBoard[newLength-1][newWidth] = (int)(fmin(gBoard[newLength-1][newWidth]+2, 0));
				printf("Hi-Ya!\n");
			}
        } // End if(gMoves[gMIndex]==-1)
		displayBoard();
		if (gameOver())
		{
			play = false;
			if (compTurn)
			{
				printf("Computer Wins!\n");
			}else
			{
				printf("You win!\n");
			}
		}
		compTurn = !compTurn;
	}
}


void displayBoard()
{
	int length, width;
	printf("\nPiece Definitions:\n");
	printf("K = +/-5\tJ = +/-4\nS = +/-3\tj = +/-2\ns = +/-1\n");
	printf("   ------------------- Computer\n");
	// Prints from gBoard[7][0] to gBoard[0][6]
	for (length = BOARD_LENGTH-1; length>=0; length--)
	{
		printf("%d", length+1);
		for (width = 0; width<BOARD_WIDTH; width++)
		{
			printf("%3d", gBoard[length][width]);
		}
		printf("\n");
	}
	printf("   ------------------- Human\n");
	printf("   A  B  C  D  E  F  G\n");
}


// Piece Definitions:
//						King = K = +/-5
//						Ninja = J = +/-4
//						Samurai = S = +/-3
//						Mini-Ninja = j = +/-2
//						Mini-Samurai = s = +/-1
// 
// Note that all length locations on printed board appear as length+1
// As such, when referring to the current piece's location, the length number will be length+1
void getMoves(bool compTurn)
{
	int length, width;
	// If there are no possible moves, gMoves[gMIndex] will be -1
	gMoves[gMIndex]=-1;
	// If it's the computer's turn, find possible moves for the computer
	// All moves described from the computer's POV
	// Computer moves from board[7][] toward gBoard[0][]
	// Left for the computer is gBoard[][0] and right is gBoard[][7]
	if (compTurn)
	{
		for (length = 0; length < BOARD_LENGTH; length++)
		{
			for (width = 0; width < BOARD_WIDTH; width++)
			{
				// Mini-Samurai
				if (gBoard[length][width]==1)
				{
					// If there is no piece in front (gBoard[length-1][]) of the Mini-Samurai, it's possible to move forward one space
					// Mini-Samurai must also not be in gBoard[0][] row
					if (length>1 && gBoard[length-1][width]==0)
						gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10) + (length-1);
					// If there is an enemy piece to the left (gBoard[][width-1]) AND forward (gBoard[length-1][]) 
					// of the Mini-Samurai AND there is no piece directly to the left (gBoard[length][width-1])...
					// Mini-Samurai must not be in gBoard[][0]
					if (width>0 && gBoard[length][width-1]==0 && gBoard[length-1][width-1]<0)
						gMoves[gMIndex++] = (width*1000) + (length*100) + (width-1)*10 + (length);
					// If there is an enemy piece to the right (gBoard[][width+1]) AND forward (gBoard[length-1][]) 
					// of the Mini-Samurai AND there is no piece directly to the right(gBoard[length][width+1])...
					// Mini-Samurai must not be in gBoard[][BOARD_WIDTH-1]
					if (width<BOARD_WIDTH-1 && gBoard[length][width+1]==0 && gBoard[length-1][width+1]<0)
						gMoves[gMIndex++] = (width*1000) + (length*100) + (width+1)*10 + (length);
				}
				// Mini-Ninja
				else if (gBoard[length][width]==2 &&  length > 0)
				{
					// If there is no piece diagonally forward to the right (gBoard[length-1][width+1]) of Mini-Ninja...
					// Mini-Ninja must not be gBoard[0][] or gBoard[][BOARD_WIDTH-1]
					if (width<BOARD_WIDTH-1 && gBoard[length-1][width+1]==0)
						gMoves[gMIndex++] = (width*1000) + (length*100) + (width+1)*10 + (length-1);
					// If there is no piece diagonally forward to the left (gBoard[length-1][width-1]) of Mini-Ninja...
					// Mini-Ninja must not be gBoard[0][] or gBoard[][0]
					if (width>0 && gBoard[length-1][width-1]==0)
						gMoves[gMIndex++] = (width*1000) + (length*100) + (width-1)*10 + (length-1);
					if (length<BOARD_LENGTH-1)
					{
						// If there is an enemy piece to the right (gBoard[][width+1]) with no piece behind it(gBoard[length+1][width+1]), 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][BOARD_WIDTH-1]
						if (width<BOARD_WIDTH-1 && gBoard[length][width+1]<0 && gBoard[length+1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width+1)*10 + (length+1);
						// If there is an enemy piece to the left (gBoard[][width-1]) with no piece behind it (gBoard[length+1][width-1]), 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][0]
						if (width>0 && gBoard[length][width-1]<0 && gBoard[length+1][width-1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width-1)*10 + (length+1);
					}
				}
				// Samurai
				else if (length>0 && gBoard[length][width]==3)
				{
					int forward;
					// Use a for loop to search for possible moves
					// Samurai can move directly forward until it runs into another piece or the end of the gBoard. 
					// Can only move horizontally to attack
					for (forward = length-1; forward>0; forward--)
					{
						if (gBoard[forward][width]==0)
						{
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10) + (forward);
						}else forward = -1; // Stop search because Samurai can't jump other pieces
					}
					// Now checking for horizontal moves to the left
					// Note that Samurai must not be in gBoard[][0]
					if (width>0)
					{
						int left;
						for (left = width-1; left>=0; left--)
						{
							// If there is nothing in the square to the left of the ninja, we can then check the square in front of it
							if (gBoard[length][left]==0)
							{
								// If there is an enemy in the square in front of the open square, then the open square becomes a possible move
								if (gBoard[length-1][left]<0)
								{
									gMoves[gMIndex++] = (width*1000) + (length*100) + (left*10) + (length);
								}
							}else left = -1; // If the square to the left is blocked, there is no need to check further
						}
					}
					// Now checking for horizontal moves to the right
					// Note that Samurai must not be in gBoard[][BOARD_WIDTH-1]
					if (width<BOARD_WIDTH-1)
					{
						int right;
						for (right = width + 1; right <BOARD_WIDTH; right++)
						{
							// If there is nothing in the square to the right of the ninja we can then check the square in front of it
							if (gBoard[length][right]==0)
							{
								// If there is an enemy in the square in front of the open square, the open square becomes a possible move
								if (gBoard[length-1][right]<0)
								{
									gMoves[gMIndex++] = (width*1000) + (length*100) + (right*10) + (length);
								}
							}else right = BOARD_WIDTH; // If the square to the right is blocked, there is no need to check further
						}
					}
				}
				// Ninja
				else if(gBoard[length][width]==4)
				{
					// Checking the forward right (gBoard[length--][width++]) directions for possible moves
					int forward = length-1;
					int right;
					for (right = width + 1; right < BOARD_WIDTH && forward > 0; right++)
					{
						if (gBoard[forward][right]==0)
						{
							gMoves[gMIndex++] = (width*1000) + (length*100) + (right*10) + (forward);
						}else 
						{
							forward = -1; // Stop search because Ninja can't jump other pieces
							right = BOARD_WIDTH;
						}
						forward--;
					}
					// Checking the forward left (gBoard[length--][width--]) directions for possible moves
					forward = length-1;
					int left;
					for (left = width - 1; left >= 0 && forward > 0; left--)
					{
						if (gBoard[forward][left]==0)
						{
							gMoves[gMIndex++] = (width*1000) + (length*100) + (left*10) + (forward);
						}else
						{
							forward = -1; // Stop search because Ninja can't jump other pieces
							left = -1;
						}
						forward--;
					}
					// Checking the backward right (gBoard[length++][width++]) for possible moves
					// Recall that a Ninja can only move backwards iff it is moving to an attack position
					int back = length + 1;
					for (right = width + 1; right < BOARD_WIDTH && back < BOARD_LENGTH; right++)
					{
						if (gBoard[back][right]==0) 
						{
							// Checking if gBoard[back][right] is an attacking position
							if (gBoard[back-1][right]<0)
							{
								gMoves[gMIndex++] = (width*1000) + (length*100) + (right*10) + (back);
							}
						}else 
						{	
							right = BOARD_WIDTH;
							back = BOARD_LENGTH;
						}
						back++;
					}
					// Checking the backward left (gBoard[length++][width--]) direction for possible moves
					// Recall that a Ninja can only move backwards iff it is moving to an attack position
					back = length + 1;
					for (left = width - 1; left >= 0 && back < BOARD_LENGTH; left--)
					{
						if (gBoard[back][left]==0)
						{
							// Checking if gBoard[back][left] is an attacking position
							if (gBoard[back-1][left]<0)
							{
								gMoves[gMIndex++] = (width*1000) + (length*100) + (left*10) + (back);
							}
						}else // If the space isn't open, stop searching because Ninja can't jump pieces
						{
							left = -1;
							back = BOARD_LENGTH;
						}
						back++;
					}
				}
			}
		}
	}
	// Finding the possible moves for the human player
	// All moves described from the computer's POV
	// Player moves from gBoard[0][] to gBoard[7][]
	// Plyer's left is gBoard[][0] and right is gBoard[][7]
	else
	{
		int length, width;
		for (length = 0; length < BOARD_LENGTH; length++)
		{
			for (width = 0; width < BOARD_WIDTH; width++)
			{
				// Mini-Samurai
				if (gBoard[length][width]==-1)
				{
					// If there is no piece in front (gBoard[length+1][]) of the Mini-Samurai, it's possible to move forward one space
					// Mini-Samurai must also not be in (gBoard[BOARD_LENGTH-1][])
					if (length<BOARD_LENGTH-2 && gBoard[length+1][width]==0)
						gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10) + (length+1);
					// If there is an enemy piece to the left (gBoard[][width-1]) AND forward (gBoard[length+1][]) of the Mini-Samurai 
					// AND there is no piece directly to the left (gBoard[length][width-1])...
					// Mini-Samurai must not be in gBoard[][0]
					if (width>0 && gBoard[length][width-1]==0 && gBoard[length+1][width-1]>0)
						gMoves[gMIndex++] = (width*1000) + (length*100) + (width-1)*10 + (length);
					// If there is an enemy piece to the right AND forward (gBoard[length+1][width+1]) of the Mini-Samurai 
					// AND there is no piece directly to the right (gBoard[length][width+1])...
					// Mini-Samurai must not be in gBoard[][BOARD_WIDTH-1]
					if (width<BOARD_WIDTH-1 && gBoard[length][width+1]==0 && gBoard[length+1][width+1]>0)
						gMoves[gMIndex++] = (width*1000) + (length*100) + (width+1)*10 + (length);
				}
				// Mini-Ninja
				else if (gBoard[length][width]==-2)
				{
					if (length<BOARD_LENGTH-1)
					{
						// If there is no piece diagonally forward to the right (gBoard[length+1][width+1]) of Mini-Ninja...
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][BOARD_WIDTH-1]
						if (width<BOARD_WIDTH-1 && gBoard[length+1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width+1)*10 + (length+1);
						// If there is no piece diagonally forward to the left (gBoard[length+1][width-1]) of Mini-Ninja...
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][0]
						if (width>0 && gBoard[length+1][width-1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width-1)*10 + (length+1);
						if (length>0)
						{
							// If there is an enemy piece to the right (gBoard[length][width+1]) with no piece behind it (gBoard[length-1][width+1]), 
							// Mini-Ninja can move there to attack
							// Mini-Ninja must not be gBoard[0][] or gBoard[][BOARD_WIDTH-1]
							if (width<BOARD_WIDTH-1 && gBoard[length][width+1]>0 && gBoard[length-1][width+1]==0)
								gMoves[gMIndex++] = (width*1000) + (length*100) + (width+1)*10 + (length-1);
							// If there is an enemy piece to the left (gBoard[length][width-1]) with no piece behind (gBoard[length-1][width-1]) it, 
							// Mini-Ninja can move there to attack
							// Mini-Ninja must not be gBoard[0][] or gBoard[][0]
							if (width>0 && gBoard[length][width-1]>0 && gBoard[length-1][width-1]==0)
								gMoves[gMIndex++] = (width*1000) + (length*100) + (width-1)*10 + (length-1);
						}
					}
				}
				// Samurai
				else if (gBoard[length][width]==-3)
				{
					int forward;
					// Use a for loop to search for possible moves
					// Samurai can move directly forward until it runs into another piece or the end of the gBoard. 
					// Can only move horizontally to attack
					for (forward = length+1; forward<BOARD_LENGTH-1; forward++)
					{
						if (gBoard[forward][width]==0)
						{
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10) + (forward);
						}else forward = BOARD_LENGTH; // Stop search because Samurai can't jump other pieces
					}
					if (length<BOARD_LENGTH-1)
					{
						int left;
						// Now checking for horizontal moves to the left
						// Note that Samurai must not be in gBoard[][0] and gBoard[BOARD_LENGTH-1][]
						for (left = width-1; left>=0; left--)
						{
							// If there is nothing in the square to the left of the ninja, we can then check the square in front of it
							if (gBoard[length][left]==0)
							{
								// If there is an enemy in the square in front of the open square, then the open square becomes a possible move
								if (gBoard[length+1][left]>0)
								{
									gMoves[gMIndex++] = (width*1000) + (length*100) + (left*10) + (length);
								}
							}else left = -1; // If the square to the left is blocked, there is no need to check further
						}
						int right;
						// Now checking for horizontal moves to the right
						// Note that Samurai must not be in gBoard[][BOARD_WIDTH-1]
						for (right = width + 1; right <BOARD_WIDTH; right++)
						{
							// If there is nothing in the square to the right of the ninja we can then check the square in front of it
							if (gBoard[length][right]==0)
							{
								// If there is an enemy in the square in front of the open square, the open square becomes a possible move
								if (gBoard[length+1][right]>0)
								{
									gMoves[gMIndex++] = (width*1000) + (length*100) + (right*10) + (length);
								}
							}else right = BOARD_WIDTH; // If the square to the right is blocked, there is no need to check further
						}
					}
				}
				// Ninja
				else if (gBoard[length][width]==-4)
				{
					// Checking the forward right (gBoard[length--][width++]) directions for possible moves
					int forward = length+1;
					int right;
					for (right = width + 1; right <BOARD_WIDTH && forward<BOARD_LENGTH; right++)
					{
						if (gBoard[forward][right]==0)
						{
							gMoves[gMIndex++] = (width*1000) + (length*100) + (right*10) + (forward);
						}else 
						{
							forward = BOARD_LENGTH; // Stop search because Ninja can't jump other pieces
							right = BOARD_WIDTH;
						}
						forward++;
					}
					// Checking the forward left (gBoard[length--][width--]) directions for possible moves
					forward = length+1;
					int left;
					for (left = width - 1; left>=0 && forward<BOARD_LENGTH; left--)
					{
						if (gBoard[forward][left]==0)
						{
							gMoves[gMIndex++] = (width*1000) + (length*100) + (left*10) + (forward);
						}else
						{
							forward = BOARD_LENGTH; // Stop search because Ninja can't jump other pieces
							left = -1;
						}
						forward++;
					}
					// Checking the backward right (gBoard[length++][width++]) for possible moves
					// Recall that a Ninja can only move backwards iff it is moving to an attack position
					int back = length - 1;
					for (right = width + 1; right < BOARD_WIDTH && back >= 0; right++)
					{
						if (gBoard[back][right]==0) 
						{
							// Checking if gBoard[back][right] is an attacking position
							if (gBoard[back+1][right]>0)
							{
								gMoves[gMIndex++] = (width*1000) + (length*100) + (right*10) + (back);
							}
						}else 
						{	
							right = BOARD_WIDTH;
							back = -1;
						}
						back--;
					}
					// Checking the backward left (gBoard[length++][width--]) direction for possible moves
					// Recall that a Ninja can only move backwards iff it is moving to an attack position
					back = length - 1;
					for (left = width - 1; left>=0 && back >= 0; left--)
					{
						if (gBoard[back][left]==0)
						{
							// Checking if gBoard[back][left] is an attacking position
							if (gBoard[back+1][left]>0)
							{
								gMoves[gMIndex++] = (width*1000) + (length*100) + (left*10) + (back);
							}
						}else // If the space isn't open, stop searching because Ninja can't jump pieces
						{
							left = -1;
							back = -1;
						}
						back--;
					}
				}
			}
		}
	}
}

// Checking if the game is over
// To begin, this just checks if there is a piece attacking each king...
bool gameOver()
{
	bool result = false;
	// If there's a piece in front of the King, the game's over
	if (gBoard[6][3]!=0)
	{
		//System.out.println("You win!!");
		result = true;
	}else if (gBoard[1][3]!=0)
	{
		//System.out.println("Computer wins!!");
		result = true;
	}else if (gMoves[gMIndex]==-1)
	{
		result = true;
	}
	return result;
}


// Beginning of the minimax algorithm
int miniMax()
{
	int moveScore, move, oldLength, newLength, oldWidth, newWidth, moveIndex;
	int decrementFlag = 0;
    int bestMove = -1;
	int bestScore = -9999;
	int stop = gMIndex;
	sort (0, gMIndex, true);
	totalNodes = gMIndex;
	pruned = 0;
	for (moveIndex = 0; moveIndex < stop; moveIndex++)
	{
		// Get the move
		move = gMoves[moveIndex];
		// Decyphering 4 digit move
		oldLength = (move%1000)/100;
		oldWidth = (move/1000);
		newLength = (move%10);
		newWidth = (move%100)/10;
		// Make the actual move
		gBoard[newLength][newWidth] = gBoard[oldLength][oldWidth];
		gBoard[oldLength][oldWidth] = 0;
		// It's an attack move if the piece in front of where the move lands is an enemy
		// Decrement the piece, increment the attack flag
		if (newLength>0 && gBoard[newLength-1][newWidth] < 0)
		{
			decrementFlag += gBoard[newLength-1][newWidth];
			gBoard[newLength-1][newWidth] += 2;
			if (gBoard[newLength-1][newWidth] > 0)
				gBoard[newLength-1][newWidth] = 0;
		}
		// Call min
		moveScore = minMiniMax(1, bestScore);
		// Retract move
		gBoard[oldLength][oldWidth] = gBoard[newLength][newWidth];
		gBoard[newLength][newWidth] = 0;
		if (decrementFlag!=0)
		{
			gBoard[newLength-1][newWidth] = decrementFlag;
			decrementFlag = 0;
		}
		// Determing the bestMove
		if (moveScore > bestScore)
		{
			bestScore = moveScore;
			bestMove = move;
			if (bestScore == EVAL_ENDING_COMP)
				moveIndex = stop;
		}
	}
	printf("Total nodes = %d\n", totalNodes);
	printf("Total prunes = %d\n", pruned);
	printf("Pruned percentage = %.2f\n", ((double)pruned/totalNodes));
	// Resetting the move index
	gMIndex = 0;
	return bestMove;
}


// Min sections tries to "predict" human move by finding the move that is best for them
// Better moves for human are more negative
int minMiniMax(int depth, int max)
{
	if (gameOver())
	{
		return EVAL_ENDING_COMP;
	}else if (depth == MAX_DEPTH)
	{
		return eval();
	}else
	{
		int bestMove, moveScore, move, oldLength, newLength, oldWidth, newWidth;
        int decrementFlag = 0;
		int humanIndex = gMIndex;
        int humanStartIndex = gMIndex;
		getMoves(false);
		int humanStopIndex = gMIndex;
		totalNodes += (humanStopIndex-humanStartIndex);
		sort(humanStartIndex, humanStopIndex, false);
		int bestScore = 9999;
		for (;humanIndex<humanStopIndex; humanIndex++)
		{
			// Get the move
			move = gMoves[humanIndex];
			// Decyphering 4 digit move
			oldLength = (move%1000)/100;
			oldWidth = (move/1000);
			newLength = (move%10);
			newWidth = (move%100)/10;
			// Make the actual move
			gBoard[newLength][newWidth] = gBoard[oldLength][oldWidth];
			gBoard[oldLength][oldWidth] = 0;
			// It's an attack move if the piece in front of where the move lands is an enemy
			// Decrement the piece, increment the attack flag
			if (newLength<BOARD_LENGTH-1 && gBoard[newLength+1][newWidth] > 0)
			{
				decrementFlag = gBoard[newLength+1][newWidth];
				gBoard[newLength+1][newWidth] -= 2;
				if (gBoard[newLength+1][newWidth] < 0)
					gBoard[newLength+1][newWidth] = 0;
			}
			moveScore = maxMiniMax((depth+1), bestScore);
			if (moveScore<bestScore)
			{
				bestScore = moveScore;
				// Alpha-Beta Pruning
				if (bestScore<max)
				{
					pruned += (humanStopIndex-humanIndex);
					humanIndex = humanStopIndex;
					humanKillerMove = move;
					historyTable[move]++;
				}
			}
			// Retract move
			gBoard[oldLength][oldWidth] = gBoard[newLength][newWidth];
			gBoard[newLength][newWidth] = 0;
			if (decrementFlag!=0)
			{
				gBoard[newLength+1][newWidth] = decrementFlag;
				decrementFlag = 0;
			}
		}
		gMIndex = humanStartIndex;
		return bestScore;
	}
}


// Max is computer's move
int maxMiniMax(int depth, int min)
{
	if (gameOver())
	{
		return EVAL_ENDING_HUM;
	}else if (depth == MAX_DEPTH)
	{
		return eval();
	}else
	{
		int bestMove, moveScore, move, oldLength, newLength, oldWidth, newWidth;
        int decrementFlag = 0;
		int compIndex = gMIndex;
        int compStartIndex = gMIndex;
		getMoves(true);
		int compStopIndex = gMIndex;
		totalNodes += (compStopIndex - compStartIndex);
		sort(compStartIndex, compStopIndex, true);
		int bestScore = -9999;
		for (;compIndex<compStopIndex; compIndex++)
		{
			// Get the move
			move = gMoves[compIndex];
			// Decyphering 4 digit move
			oldLength = (move%1000)/100;
			oldWidth = (move/1000);
			newLength = (move%10);
			newWidth = (move%100)/10;
			// Make the actual move
			gBoard[newLength][newWidth] = gBoard[oldLength][oldWidth];
			gBoard[oldLength][oldWidth] = 0;
			// It's an attack move if the piece in front of where the move lands is an enemy
			// Decrement the piece, increment the attack flag
           	if (newLength>0 && gBoard[newLength-1][newWidth] < 0)
				{
					decrementFlag += gBoard[newLength-1][newWidth];
					gBoard[newLength-1][newWidth] += 2;
					if (gBoard[newLength-1][newWidth] > 0)
						gBoard[newLength-1][newWidth] = 0;
				}
			moveScore = minMiniMax((depth+1), bestScore);
			// Retract move
			gBoard[oldLength][oldWidth] = gBoard[newLength][newWidth];
			gBoard[newLength][newWidth] = 0;
			if (decrementFlag!=0)
			{
				gBoard[newLength-1][newWidth] = decrementFlag;
				decrementFlag = 0;
			}
			if (moveScore>bestScore)
			{
				if (bestScore == EVAL_ENDING_COMP)
					compIndex = compStopIndex;
				bestScore = moveScore;
				// Alpha-Beta Pruning
				if (bestScore>min)
				{
					pruned += (compStopIndex-compIndex);
					compIndex = compStopIndex;
					compKillerMove = move;
					historyTable[move]++;
				}
			}
		}
		gMIndex = compStartIndex;
		return bestScore;
	}
}


// Heuristic
int eval()
{
	int result = 0;
	int piece, length, width;
	for (length = 0; length < BOARD_LENGTH; length++)
	{
		for (width = 0; width < BOARD_WIDTH; width++)
		{
			piece = gBoard[length][width];
			if (piece!=5 && piece!=-5 && piece!=0)
			{
				// If it's a human piece
				if (piece<0)
				{
					// Add the total distance from a kill move
					result += piece - (abs(length - BOARD_LENGTH+2) - abs(width-3));
					// Not all ninja-type pieces can actually attack king
					// Place additional weight on those that can
					if ((piece == -2 || piece == -4) && (length+width)%2!=0)
						result += abs(piece)*piece;
					// Place major weight on Samurai that have a clear shot at the King
					else if (piece == -3)
					{
						result += 2*(abs(piece)*piece);
						int i = 1;
						if (width==3)
						{
							while (length<BOARD_LENGTH-1 && gBoard[length+(i)][width]==0) i++;
							if (length+i == BOARD_LENGTH-2)
								result += piece*5;
						}
					}
				}else if (piece>0) //Computer piece
				{
					result += piece + ((length-1) - abs(width-3));
					// Not all ninja-type pieces can actually attack king
					// Place additional weight on those that can
					if ((piece == 2 || piece == 4) && (length+width)%2==0)
						result += piece*piece;
					// Place major weight on Samurai that have a clear shot at the King
					else if (piece == 3)
					{
						result += 2*piece*piece;
						int i = 1;
						if (width == 3)
						{
							while (length>0 && gBoard[length-i][width]==0) i++;
							if (length-i == 1)
								result += piece*5;
						}
					}

				}
			}
		}
	}
	return result;
}



void sort(int start, int stop, bool compTurn)
{
	int move;
	if (compTurn)
	{
		for (move = start;move<stop;move++)
		{
			if(gMoves[move]==compKillerMove)
			{
				int temp = gMoves[start];
				gMoves[start++] = gMoves[move];
				gMoves[move] = temp;
				move = stop;
			}

			/*else if (historyTable[gMoves[move]]>0)
			{
				int temp = gMoves[start];
				gMoves[start++] = gMoves[move];
				gMoves[move] = temp;
			}*/
		}
	}else
	{
		for (move = start;move<stop;move++)
		{
			if(gMoves[move]==humanKillerMove)
			{
				int temp = gMoves[start];
				gMoves[start++] = gMoves[move];
				gMoves[move] = temp;
				move = stop;
			}
		}
	}
}