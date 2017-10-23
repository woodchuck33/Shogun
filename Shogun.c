/*
Author: Joe Ezaki
		Shogun
		Project 2
		CSc 180
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Minimax constants
#define EVAL_ENDING_COMP 999
#define EVAL_ENDING_HUM -999
#define MAX_DEPTH 8

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
int min(int depth, int max);
int max(int depth, int min);
int eval();
void sort(int start, int stop, bool compTurn);

int main()
{
	gMIndex = 0;
	playGame();
}


void playGame()
{
	bool play = true;
	bool compTurn = false;
	bool legal = false;
	char input;
	int counter =0;
	printf("Do you want to go first (y/n)? ");
	scanf("%c", &input);
	if (input == 'n')
		compTurn = true;
	//displayBoard();
	//while (play)
	//{
		
	//}
}


void displayBoard()
{
	printf("\nPiece Definitions:");
	printf("K = +/-5\tJ = +/-4\nS = +/-3\tj = +/-2\ns = +/-1");
	printf("   ------------------- Computer");
	// Prints from gBoard[7][0] to gBoard[0][6]
	for (int length = BOARD_LENGTH-1; length>=0; length--)
	{
		System.out.print (length+1);
		for (int width = 0; width<BOARD_WIDTH; width++)
		{
			printf("%3d", gBoard[length][width]);
		}
		printf();
	}
	printf("   ------------------- Human");
	printf("   A  B  C  D  E  F  G");
}