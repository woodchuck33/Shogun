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
#define MAX_DEPTH 9

// Board size constants
#define BOARD_LENGTH 8
#define BOARD_WIDTH 7
#define MAX_MOVES 20000

// Global variables
int gBoard[80] = {0, 0, 0, -5, 0, 0, 0, 0, 0, 0,
				 -3, -3, -3, 0, -4, -4, -4, 0, 0, 0,
				 -2, -2, -2, 0, -1, -1, -1, 0, 0, 0,
				  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				  1, 1, 1, 0, 2, 2, 2, 0, 0, 0,
				  4, 4, 4, 0, 3, 3, 3, 0, 0, 0, 
				  0, 0, 0, 5, 0, 0, 0, 0, 0, 0};
/*int gBoard[BOARD_LENGTH][BOARD_WIDTH] = {{0, 0, 0, -5, 0, 0, 0},
			 {-3, -3, -3, 0, -4, -4, -4},
			 {-2, -2, -2, 0, -1, -1, -1},
			 {0, 0, 0, 0, 0, 0, 0},
			 {0, 0, 0, 0, 0, 0, 0},
			 {1, 1, 1, 0, 2, 2, 2},
			 {4, 4, 4, 0, 3, 3, 3},
			 {0, 0, 0, 5, 0, 0, 0}}; // The board*/
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
        int oldSpot, newSpot;
		getMoves(compTurn);
        if (gMoves[0]>0)
        {
			if (compTurn)
			{	
				start = time(NULL);
				move = miniMax();
				stop = time(NULL);
				printf("Total search time: %f\n", difftime(stop, start));
				moveString[0] = (char)((move%1000/100)+'A');
				moveString[1] = (char)((move/1000)+49);
				moveString[2] = (char)((move%10)+'A');
				moveString[3] = (char)((move%100/10)+49);
				printf("My move is: %s ", moveString);
				moveString[0] = (char)(abs((move%1000/100)-6)+'A');
				moveString[1] = (char)(abs((move/1000)-8) + 48);
				moveString[2] = (char)(abs((move%10)-6)+'A');
				moveString[3] = (char)(abs((move%100/10)-8)+48);
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
						moveString[0] = (char)((move%1000/100)+'A');
						moveString[1] = (char)((move/1000)+49);
						moveString[2] = (char)((move%10)+'A');
						moveString[3] = (char)((move%100/10)+49);
						printf("%s ", moveString);
					}
					printf("\n");
					if (scanf("%s", codedMove)!=1) printf("Uh-oh\n");
					move = (100*(int)(toupper(codedMove[0])-'A')) + (1000*(int)(codedMove[1]-'1')) + ((int)(toupper(codedMove[2])-'A')) + 10*((int)(codedMove[3]-'1'));
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
			oldSpot = move/100;
			newSpot = (move%100);
			// Make the actual move
			gBoard[newSpot] = gBoard[oldSpot];
			gBoard[oldSpot] = 0;
			// It's an attack move if the piece in front of where the move lands is an enemy
			// Decrement the piece, increment the attack flag
			if (newSpot < 70 && !compTurn && gBoard[newSpot+10] > 0 && gBoard[newSpot+10] != 5)
			{	
				gBoard[newSpot+10] = ((gBoard[newSpot+10]-2)>0)? gBoard[newSpot+10]-2:0;
			}
			// Computer's attack
			else if(compTurn && newSpot> 9 && gBoard[newSpot-10] < 0 && gBoard[newSpot-10] != -5)
			{
				gBoard[newSpot-10] = ((gBoard[newSpot-10]+2)<0)? (gBoard[newSpot-10]+2):0;
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
	int spot;
	printf("\nPiece Definitions:\n");
	printf("K = +/-5\tJ = +/-4\nS = +/-3\tj = +/-2\ns = +/-1\n");
	printf("   ------------------- Computer\n");
	// Prints from gBoard[7][0] to gBoard[0][6]
	for (spot = 70; spot>=0; spot-=10)
	{
		printf("%d", (spot+10)/10);
		printf("%3d%3d%3d%3d%3d%3d%3d\n", gBoard[spot], gBoard[spot+1], gBoard[spot+2], gBoard[spot+3], gBoard[spot+4], gBoard[spot+5], gBoard[spot+6]);
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
	int spot=0;
	// If there are no possible moves, gMoves[gMIndex] will be -1
	gMoves[gMIndex]=-1;
	// If it's the computer's turn, find possible moves for the computer
	// All moves described from the computer's POV
	// Computer moves from board[7][] toward gBoard[0][]
	// Left for the computer is gBoard[][0] and right is gBoard[][7]
	if (compTurn)
	{
		for (spot = 0; spot < 77; spot++)
		{
			if (spot%10==7) spot +=3;
			// Mini-Samurai
			if (gBoard[spot]==1 && spot > 9)
			{
				// If there is no piece in front (gBoard[length-1][]) of the Mini-Samurai, it's possible to move forward one space
				// Mini-Samurai must also not be in gBoard[0][] row
				if (gBoard[spot-10]==0)
					gMoves[gMIndex++] = (spot*100) + (spot-10);
				// If there is an enemy piece to the left (gBoard[][width-1]) AND forward (gBoard[length-1][]) 
				// of the Mini-Samurai AND there is no piece directly to the left (gBoard[length][width-1])...
				// Mini-Samurai must not be in gBoard[][0]
				if (spot%10>0 && gBoard[spot-1]==0 && gBoard[spot-11]<0)
					gMoves[gMIndex++] = (spot*100) + (spot-1);
				// If there is an enemy piece to the right (gBoard[][width+1]) AND forward (gBoard[length-1][]) 
				// of the Mini-Samurai AND there is no piece directly to the right(gBoard[length][width+1])...
				// Mini-Samurai must not be in gBoard[][BOARD_WIDTH-1]
				if (spot%10<BOARD_WIDTH-1 && gBoard[spot+1]==0 && gBoard[spot-9]<0)
					gMoves[gMIndex++] = (spot*100) + (spot+1);
			}
			// Mini-Ninja
			else if (gBoard[spot]==2)
			{
				// If there is no piece diagonally forward to the right (gBoard[length-1][width+1]) of Mini-Ninja...
				// Mini-Ninja must not be gBoard[0][] or gBoard[][BOARD_WIDTH-1]
				if (spot>9 && spot%10<BOARD_WIDTH-1 && gBoard[spot-9]==0)
					gMoves[gMIndex++] = (spot*100) + (spot-9);
				// If there is no piece diagonally forward to the left (gBoard[length-1][width-1]) of Mini-Ninja...
				// Mini-Ninja must not be gBoard[0][] or gBoard[][0]
				if (spot>10 && spot%10>0 && gBoard[spot-11]==0)
					gMoves[gMIndex++] = (spot*100) + (spot-11);
				if (spot/10<BOARD_LENGTH-1)
				{
					// If there is an enemy piece to the right (gBoard[][width+1]) with no piece behind it(gBoard[length+1][width+1]), 
					// Mini-Ninja can move there to attack
					// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][BOARD_WIDTH-1]
					if (spot%10<BOARD_WIDTH-1 && gBoard[spot+1]<0 && gBoard[spot+11]==0)
						gMoves[gMIndex++] = (spot*100) + (spot+11);
					// If there is an enemy piece to the left (gBoard[][width-1]) with no piece behind it (gBoard[length+1][width-1]), 
					// Mini-Ninja can move there to attack
					// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][0]
					if (spot%10>0 && gBoard[spot-1]<0 && gBoard[spot+9]==0)
						gMoves[gMIndex++] = (spot*100) + (spot+9);
				}
			}
			// Samurai
			else if (gBoard[spot]==3 && spot>9)
			{
				int forward;
				// Use a for loop to search for possible moves
				// Samurai can move directly forward until it runs into another piece or the end of the gBoard. 
				// Can only move horizontally to attack
				for (forward = spot-10; forward>0; forward-=10)
				{
					if (gBoard[forward]==0)
					{
						gMoves[gMIndex++] = (spot*100) + forward;
					}else forward = -1; // Stop search because Samurai can't jump other pieces
				}
				// Now checking for horizontal moves to the left
				// Note that Samurai must not be in gBoard[][0]
				if (spot%10>0)
				{
					int left;
					for (left = spot-1; left%10<BOARD_WIDTH; left--)
					{
						// If there is nothing in the square to the left of the ninja, we can then check the square in front of it
						if (gBoard[left]==0)
						{
							// If there is an enemy in the square in front of the open square, then the open square becomes a possible move
							if (gBoard[left-10]<0)
							{
								gMoves[gMIndex++] = (spot*100) + left;
							}
						}else left = BOARD_WIDTH+1; // If the square to the left is blocked, there is no need to check further
					}
				}
				// Now checking for horizontal moves to the right
				// Note that Samurai must not be in gBoard[][BOARD_WIDTH-1]
				if (spot%10<BOARD_WIDTH-1)
				{
					int right;
					for (right = spot + 1; right%10 <BOARD_WIDTH; right++)
					{
						// If there is nothing in the square to the right of the ninja we can then check the square in front of it
						if (gBoard[right]==0)
						{
							// If there is an enemy in the square in front of the open square, the open square becomes a possible move
							if (gBoard[right-10]<0)
							{
								gMoves[gMIndex++] = (spot*100) + right;
							}
						}else right = BOARD_WIDTH; // If the square to the right is blocked, there is no need to check further
					}
				}
			}
			// Ninja
			else if(gBoard[spot]==4)
			{
				// Checking the forward right (gBoard[length--][width++]) directions for possible moves
				int fRight;
				for (fRight = spot-9; fRight%10 < BOARD_WIDTH && fRight > 0; fRight-=9)
				{
					if (gBoard[fRight]==0)
					{
						gMoves[gMIndex++] = (spot*100) + (fRight);
					}else 
					{
						fRight = -1; // Stop search because Ninja can't jump other pieces
					}
				}
				// Checking the forward left (gBoard[length--][width--]) directions for possible moves
				int fLeft;
				for (fLeft = spot - 11; fLeft >= 0 && fLeft%10 <BOARD_WIDTH; fLeft-=11)
				{
					if (gBoard[fLeft]==0)
					{
						gMoves[gMIndex++] = (spot*100) + (fLeft);
					}else
					{
						fLeft = -1; // Stop search because Ninja can't jump other pieces
					}
				}
				// Checking the backward right (gBoard[length++][width++]) for possible moves
				// Recall that a Ninja can only move backwards iff it is moving to an attack position
				int bRight;
				for (bRight = spot + 11; bRight%10 < BOARD_WIDTH && bRight <= 76; bRight+=11)
				{
					if (gBoard[bRight]==0) 
					{
						// Checking if gBoard[back][right] is an attacking position
						if (gBoard[bRight-10]<0)
						{
							gMoves[gMIndex++] = (spot*100) + bRight;
						}
					}else 
					{	
						break;
					}
				}
				// Checking the backward left (gBoard[length++][width--]) direction for possible moves
				// Recall that a Ninja can only move backwards iff it is moving to an attack position
				int bLeft;
				for (bLeft = spot + 9; bLeft%10 <BOARD_WIDTH && bLeft < 76; bLeft+=9)
				{
					if (gBoard[bLeft]==0)
					{
						// Checking if gBoard[back][left] is an attacking position
						if (gBoard[bLeft-10]<0)
						{
							gMoves[gMIndex++] = (spot*100) + bLeft;
						}
					}else // If the space isn't open, stop searching because Ninja can't jump pieces
					{
						break;
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
		for (spot = 0; spot < 77; spot++)
		{
			if (spot%10==7) spot +=3;
			
			// Mini-Samurai
			if (gBoard[spot]==-1 && spot < 70)
			{
				// If there is no piece in front (gBoard[length+1][]) of the Mini-Samurai, it's possible to move forward one space
				// Mini-Samurai must also not be in (gBoard[BOARD_LENGTH-1][])
				if (gBoard[spot+10]==0)
					gMoves[gMIndex++] = (spot*100) + (spot+10);
				// If there is an enemy piece to the left (gBoard[][width-1]) AND forward (gBoard[length+1][]) of the Mini-Samurai 
				// AND there is no piece directly to the left (gBoard[length][width-1])...
				// Mini-Samurai must not be in gBoard[][0]
				if (spot%10>0 && gBoard[spot-1]==0 && gBoard[spot+9]>0)
					gMoves[gMIndex++] = (spot*100) + (spot-1);
				// If there is an enemy piece to the right AND forward (gBoard[length+1][width+1]) of the Mini-Samurai 
				// AND there is no piece directly to the right (gBoard[length][width+1])...
				// Mini-Samurai must not be in gBoard[][BOARD_WIDTH-1]
				if (spot%10<BOARD_WIDTH-1 && gBoard[spot+1]==0 && gBoard[spot+11]>0)
					gMoves[gMIndex++] = (spot*100) + (spot+1);
			}
			// Mini-Ninja
			else if (gBoard[spot]==-2)
			{
				if (spot<70)
				{
					// If there is no piece diagonally forward to the right (gBoard[length+1][width+1]) of Mini-Ninja...
					// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][BOARD_WIDTH-1]
					if (spot%10<BOARD_WIDTH-1 && gBoard[spot+11]==0)
						gMoves[gMIndex++] = (spot*100) + (spot+11);
					// If there is no piece diagonally forward to the left (gBoard[length+1][width-1]) of Mini-Ninja...
					// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][0]
					if (spot%10<BOARD_WIDTH && gBoard[spot+9]==0)
					gMoves[gMIndex++] = (spot*100) + (spot+9);
					// If there is an enemy piece to the right (gBoard[length][width+1]) with no piece behind it (gBoard[length-1][width+1]), 
					// Mini-Ninja can move there to attack
					// Mini-Ninja must not be gBoard[0][] or gBoard[][BOARD_WIDTH-1]
					if (spot>9 && spot%10<BOARD_WIDTH-1 && gBoard[spot+1]>0 && gBoard[spot-9]==0)
						gMoves[gMIndex++] = (spot*100) + (spot-9);
					// If there is an enemy piece to the left (gBoard[length][width-1]) with no piece behind (gBoard[length-1][width-1]) it, 
					// Mini-Ninja can move there to attack
					// Mini-Ninja must not be gBoard[0][] or gBoard[][0]
					if (spot>10 && spot%10<BOARD_WIDTH && gBoard[spot-1]>0 && gBoard[spot-11]==0)
						gMoves[gMIndex++] = (spot*100) + (spot-11);
				}
			}
			// Samurai
			else if (gBoard[spot]==-3)
			{
				int forward;
				// Use a for loop to search for possible moves
				// Samurai can move directly forward until it runs into another piece or the end of the gBoard. 
				// Can only move horizontally to attack
				for (forward = spot+10; forward<77; forward+=10)
				{
					if (gBoard[forward]==0)
					{
						gMoves[gMIndex++] = (spot*100) + forward;
					}else forward = 80; // Stop search because Samurai can't jump other pieces
				}
				int left;
				// Now checking for horizontal moves to the left
				// Note that Samurai must not be in gBoard[][0] and gBoard[BOARD_LENGTH-1][]
				if(spot<70)
				{
					for (left = spot-1; left%10<BOARD_WIDTH; left--)
					{
						// If there is nothing in the square to the left of the ninja, we can then check the square in front of it
						if (gBoard[left]==0)
						{
							// If there is an enemy in the square in front of the open square, then the open square becomes a possible move
							if (gBoard[left+10]>0)
							{
								gMoves[gMIndex++] = (spot*100) + left;
							}
						}else left = BOARD_WIDTH+1; // If the square to the left is blocked, there is no need to check further
					}
					int right;
					// Now checking for horizontal moves to the right
					// Note that Samurai must not be in gBoard[][BOARD_WIDTH-1]
					for (right = spot + 1; right%10<BOARD_WIDTH; right++)
					{
						// If there is nothing in the square to the right of the ninja we can then check the square in front of it
						if (gBoard[right]==0)
						{
							// If there is an enemy in the square in front of the open square, the open square becomes a possible move
							if (gBoard[right+10]>0)
							{
								gMoves[gMIndex++] = (spot*100) + right;
							}
						}else right = BOARD_WIDTH+1; // If the square to the right is blocked, there is no need to check further
					}
				}
			}
			// Ninja
			else if (gBoard[spot]==-4)
			{
				// Checking the forward right (gBoard[length--][width++]) directions for possible moves
				int fRight;
				for (fRight = spot+ 11; fRight%10 <BOARD_WIDTH && fRight<77; fRight+=11)
				{
					if (gBoard[fRight]==0)
					{
						gMoves[gMIndex++] = (spot*100) + (fRight);
					}else 
					{
						fRight = BOARD_WIDTH+1; // Stop search because Ninja can't jump other pieces
					}
				}
				// Checking the forward left (gBoard[length--][width--]) directions for possible moves
				int fLeft;
				for (fLeft = spot+9; fLeft%10<BOARD_WIDTH && fLeft<77; fLeft+=9)
				{
					if (gBoard[fLeft]==0)
					{
						gMoves[gMIndex++] = (spot*100) + (fLeft);
					}else
					{
						fLeft = BOARD_WIDTH+1; // Stop search because Ninja can't jump other pieces
					}
				}
				// Checking the backward right (gBoard[length++][width++]) for possible moves
				// Recall that a Ninja can only move backwards iff it is moving to an attack position
				int bRight;
				for (bRight =spot-9; bRight%10 < BOARD_WIDTH && bRight >= 0; bRight-=9)
				{
					if (gBoard[bRight]==0) 
					{
						// Checking if gBoard[back][right] is an attacking position
						if (gBoard[bRight+10]>0)
						{
							gMoves[gMIndex++] = (spot*100) + bRight;
						}
					}else 
					{	
						bRight = BOARD_WIDTH+1;
					}
				}
				// Checking the backward left (gBoard[length++][width--]) direction for possible moves
				// Recall that a Ninja can only move backwards iff it is moving to an attack position
				int bLeft;
				for (bLeft = spot-11; bLeft>=0 && bLeft%10< BOARD_WIDTH; bLeft-=11)
				{
					if (gBoard[bLeft]==0)
					{
						// Checking if gBoard[back][left] is an attacking position
						if (gBoard[bLeft+10]>0)
						{
							gMoves[gMIndex++] = (spot*100) + bLeft;
						}
					}else // If the space isn't open, stop searching because Ninja can't jump pieces
					{
						bLeft = -1;
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
	if (gBoard[63]!=0)
	{
		//System.out.println("You win!!");
		result = true;
	}else if (gBoard[13]!=0)
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
	int moveScore, move, newSpot, oldSpot, moveIndex;
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
		oldSpot = move/100;
		newSpot = move%100;
		// Make the actual move
		gBoard[newSpot] = gBoard[oldSpot];
		gBoard[oldSpot] = 0;
		// It's an attack move if the piece in front of where the move lands is an enemy
		// Decrement the piece, increment the attack flag
		if (newSpot>9 && gBoard[newSpot-10] < 0)
		{
			decrementFlag += gBoard[newSpot-10];
			gBoard[newSpot-10] += 2;
			if (gBoard[newSpot-10] > 0)
				gBoard[newSpot-10] = 0;
		}
		// Call min
		moveScore = minMiniMax(1, bestScore);
		// Retract move
		gBoard[oldSpot] = gBoard[newSpot];
		gBoard[newSpot] = 0;
		if (decrementFlag!=0)
		{
			gBoard[newSpot-10] = decrementFlag;
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
		int bestMove, moveScore, move, oldSpot, newSpot;
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
			oldSpot = move/100;
			newSpot = (move%100);
			// Make the actual move
			gBoard[newSpot] = gBoard[oldSpot];
			gBoard[oldSpot] = 0;
			// It's an attack move if the piece in front of where the move lands is an enemy
			// Decrement the piece, increment the attack flag
			if (newSpot<70 && gBoard[newSpot+10] > 0)
			{
				decrementFlag = gBoard[newSpot+10];
				gBoard[newSpot+10] -= 2;
				if (gBoard[newSpot+10] < 0)
					gBoard[newSpot+10] = 0;
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
			gBoard[oldSpot] = gBoard[newSpot];
			gBoard[newSpot] = 0;
			if (decrementFlag!=0)
			{
				gBoard[newSpot+10] = decrementFlag;
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
		int bestMove, moveScore, move, oldSpot, newSpot;
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
			oldSpot = move/100;
			newSpot = (move%100);
			// Make the actual move
			gBoard[newSpot] = gBoard[oldSpot];
			gBoard[oldSpot] = 0;
			// It's an attack move if the piece in front of where the move lands is an enemy
			// Decrement the piece, increment the attack flag
           	if (newSpot>9 && gBoard[newSpot-10] < 0)
				{
					decrementFlag += gBoard[newSpot-10];
					gBoard[newSpot-10] += 2;
					if (gBoard[newSpot-10] > 0)
						gBoard[newSpot-10] = 0;
				}
			moveScore = minMiniMax((depth+1), bestScore);
			// Retract move
			gBoard[oldSpot] = gBoard[newSpot];
			gBoard[newSpot] = 0;
			if (decrementFlag!=0)
			{
				gBoard[newSpot-10] = decrementFlag;
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
	int piece, spot;
	for (spot = 0; spot < 77; spot++)
	{
		piece = gBoard[spot];
		if (piece!=0 && piece!=5 && piece!=-5)
		{
			// If it's a human piece
			if (piece<0)
			{
				// Add the total distance from a kill move
				result += piece - (abs(spot/10 - BOARD_LENGTH+2) - abs(spot%10-3));
				// Not all ninja-type pieces can actually attack king
				// Place additional weight on those that can
				if ((piece == -2 || piece == -4) && (spot%10+spot/10)%2!=0)
					result += abs(piece)*piece;
				// Place major weight on Samurai that have a clear shot at the King
				else if (piece == -3)
				{
					result += 2*(abs(piece)*piece);
					int i = 10;
					if (spot%10==3)
					{
						while (spot+i<70 && gBoard[spot+(i)]==0) i+=10;
						if (spot+i == 63)
							result += piece*5;
					}else if((spot+i)/10==6)
					{
						result += piece*5;
					}
				}
			}else if (piece>0) //Computer piece
			{
				result += piece + ((spot/10-1) - abs(spot%10-3));
				// Not all ninja-type pieces can actually attack king
				// Place additional weight on those that can
				if ((piece == 2 || piece == 4) && (spot%10+spot/10)%2==0)
					result += piece*piece;
				// Place major weight on Samurai that have a clear shot at the King
				else if (piece == 3)
				{
					result += 2*piece*piece;
					int i = 10;
					if (spot%10 == 3)
					{
						while (spot>9 && gBoard[spot-i]==0) i+=10;
						if (spot-i== 13)
							result += piece*5;
					}else if((spot-i)/10==1)
					{
						result += piece*5;
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