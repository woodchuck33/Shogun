/* Author:	Joe Ezaki
   CSc 180
   */

import java.util.Scanner;

public class Shogun
{
	// Minimax constanst
	private static final int EVAL_ENDING_COMP = 999;
	private static final int EVAL_ENDING_HUM = -999;
	private static final int MAX_DEPTH = 7;
	
	// Board size constants
	private static final int BOARD_LENGTH=8;
	private static final int BOARD_WIDTH=7;

	// Global variables
	private static int[][] gBoard; // The board
	private static int[] gMoves; // Array of possible moves
	private static int gMIndex;
	
	public static void main(String[] args)
	{
		Scanner kb = new Scanner(System.in);
		initBoard();
		playGame(kb);
	}

	public static void playGame(Scanner kb)
	{
		boolean play = true;
		boolean compTurn = false;
		boolean legal = false;
		int counter =0;
		System.out.print("Do you want to go first (y/n)? ");
		compTurn = kb.next().equalsIgnoreCase("y")?false:true;
		displayBoard();
		while (play)
		{
			int move = -1;
            int oldLength, oldWidth, newLength, newWidth;
			getMoves(compTurn);
            if (gMIndex>0)
            {
    			if (compTurn)
    			{	
    				move = miniMax();
    				System.out.print("My move is: " + (char)((move/1000)+'A') + ((move%1000/100)+1) + (char)((move%100/10)+'A') + ((move%10)+1) + " ");
    			}
    			// Human player's turn
    			else
    			{
    				legal = false;
    				while(!legal)
    				{
    					int i;
    					System.out.println("Please choose one of these possible moves: ");
    					for (i = 0; i < gMIndex; i++)
    					{
    						move = gMoves[i];
    						System.out.print(move + "=" + (char)((move/1000)+'A') + ((move%1000/100)+1) + (char)((move%100/10)+'A') + ((move%10)+1) + " ");
    					}
    					System.out.println();
    					String codedMove = kb.next();
    					move = (1000*(int)(Character.toUpperCase(codedMove.charAt(0))-'A')) + (100*(int)(codedMove.charAt(1)-'1')) + (10*(int)(Character.toUpperCase(codedMove.charAt(2))-'A')) + (int)(codedMove.charAt(3)-'1');
    					System.out.println("Converted Move " + move);
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
    						System.out.println("That's not a legal move.  Try again.");
    				}
                    gMIndex = 0;
    			}
    			// Decyphering 4 digit move
    			oldLength = (move%1000)/100;
    			oldWidth = (move/1000);
    			newLength = (move%10);
    			newWidth = (move%100)/10;
    			// Make the actual move
    			System.out.printf("Old Length: %d\nOld Width: %d\nNew Length: %d\nNew Width: %d\n", oldLength, oldWidth, newLength, newWidth);
    			System.out.printf("Value of old location: %d", gBoard[oldLength][oldWidth]);
    			gBoard[newLength][newWidth] = gBoard[oldLength][oldWidth];
    			gBoard[oldLength][oldWidth] = 0;
    			// It's an attack move if the piece in front of where the move lands is an enemy
    			// Decrement the piece, increment the attack flag
    			if (!compTurn && gBoard[newLength+1][newWidth] > 0 && gBoard[newLength+1][newWidth] != 5)
    			{	
    				gBoard[newLength+1][newWidth]--;
    			}else if(gBoard[newLength+1][newWidth] < 0 && gBoard[newLength+1][newWidth] != -5)
    			{
    				gBoard[newLength-1][newWidth]++;
    			}
            }
			displayBoard();
			compTurn = !compTurn;
			if (gameOver())
				play = false;
		}
	}


	// Piece Definitions:
	//						King = K = +/-5
	//						Ninja = J = +/-4
	//						Samurai = S = +/-3
	//						Mini-Ninja = j = +/-2
	//						Mini-Samurai = s = +/-1
	public static void initBoard()
	{
		gBoard = new int[][]{{0, 0, 0, -5, 0, 0, 0},
				 {-3, -3, -3, 0, -4, -4, -4},
				 {-2, -2, -2, 0, -1, -1, -1},
				 {0, 0, 0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0, 0, 0},
				 {1, 1, 1, 0, 2, 2, 2},
				 {4, 4, 4, 0, 3, 3, 3},
				 {0, 0, 0, 5, 0, 0, 0}};
		// Maximum number of possible moves at any given time for both players is approx. 186...
		// This is a gross overestimation for the number of moves needed at 20 plies deep
		gMoves = new int[20000];
		gMIndex = 0;
	}

	public static void displayBoard()
	{
		// Prints from gBoard[7][0] to gBoard[0][6]
		for (int length = BOARD_LENGTH-1; length>=0; length--)
		{
			System.out.print(length + 1);
			for (int width = 0; width<BOARD_WIDTH; width++)
			{
				System.out.printf("%3d", gBoard[length][width]);
			}
			System.out.println();
		}
		System.out.println("  A  B  C  D  E  F  G");
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
	public static void getMoves(boolean compTurn)
	{
		System.out.println("Getting legal moves...");
		// If it's the computer's turn, find possible moves for the computer
		// All moves described from the computer's POV
		// Computer moves from board[7][] toward gBoard[0][]
		// Left for the computer is gBoard[][0] and right is gBoard[][7]
		if (compTurn)
		{
			for (int length = 0; length < BOARD_LENGTH; length++)
			{
				for (int width = 0; width < BOARD_WIDTH; width++)
				{
					// Mini-Samurai
					if (gBoard[length][width]==1)
					{
						// If there is no piece in front (gBoard[length-1][]) of the Mini-Samurai, it's possible to move forward one space
						// Mini-Samurai must also not be in gBoard[0][] row
						if (length>0 && gBoard[length-1][width]==0)
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
					else if (gBoard[length][width]==2)
					{
						// If there is no piece diagonally forward to the right (gBoard[length-1][width+1]) of Mini-Ninja...
						// Mini-Ninja must not be gBoard[0][] or gBoard[][BOARD_WIDTH-1]
						if (length>0 && width<BOARD_WIDTH-1 && gBoard[length-1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width+1)*10 + (length-1);
						// If there is no piece diagonally forward to the left (gBoard[length-1][width-1]) of Mini-Ninja...
						// Mini-Ninja must not be gBoard[0][] or gBoard[][0]
						if (length>0 && width>0 && gBoard[length-1][width-1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width-1)*10 + (length-1);
						// If there is an enemy piece to the right (gBoard[][width+1]) with no piece behind it(gBoard[length+1][width+1]), 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][BOARD_WIDTH-1]
						if (length<BOARD_LENGTH-1 && width<BOARD_WIDTH-1 && gBoard[length][width+1]<0 && gBoard[length+1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width+1)*10 + (length+1);
						// If there is an enemy piece to the left (gBoard[][width-1]) with no piece behind it (gBoard[length+1][width-1]), 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][0]
						if (length<BOARD_LENGTH-1 && width>0 && gBoard[length][width-1]<0 && gBoard[length+1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width-1)*10 + (length+1);
					}
					// Samurai
					else if (length>0 && gBoard[length][width]==3)
					{
						// Use a for loop to search for possible moves
						// Samurai can move directly forward until it runs into another piece or the end of the gBoard. 
						// Can only move horizontally to attack
						for (int forward = length-1; forward>=0; forward--)
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
							for (int left = width-1; left>=0; left--)
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
							for (int right = width + 1; right <BOARD_WIDTH; right++)
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
						for (int right = width + 1; right < BOARD_WIDTH && forward > 0; right++)
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
						for (int left = width - 1; left >= 0 && forward > 0; left--)
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
						for (int right = width + 1; right < BOARD_WIDTH && back < BOARD_LENGTH; right++)
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
						for (int left = width - 1; left >= 0 && back < BOARD_LENGTH; left--)
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
			for (int length = 0; length < BOARD_LENGTH; length++)
			{
				for (int width = 0; width < BOARD_WIDTH; width++)
				{
					// Mini-Samurai
					if (gBoard[length][width]==-1)
					{
						// If there is no piece in front (gBoard[length+1][]) of the Mini-Samurai, it's possible to move forward one space
						// Mini-Samurai must also not be in (gBoard[BOARD_LENGTH-1][])
						if (length<BOARD_LENGTH-1 && gBoard[length+1][width]==0)
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
						// If there is no piece diagonally forward to the right (gBoard[length+1][width+1]) of Mini-Ninja...
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][BOARD_WIDTH-1]
						if (length<BOARD_LENGTH-1 && width<BOARD_WIDTH-1 && gBoard[length+1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width+1)*10 + (length+1);
						// If there is no piece diagonally forward to the left (gBoard[length+1][width-1]) of Mini-Ninja...
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][0]
						if (length<BOARD_LENGTH-1 && width>0 && gBoard[length+1][width-1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width-1)*10 + (length+1);
						// If there is an enemy piece to the right (gBoard[length][width+1]) with no piece behind it (gBoard[length-1][width+1]), 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be gBoard[0][] or gBoard[][BOARD_WIDTH-1]
						if (length>0 && width<BOARD_WIDTH-1 && gBoard[length][width+1]>0 && gBoard[length-1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width+1)*10 + (length-1);
						// If there is an enemy piece to the left (gBoard[length][width-1]) with no piece behind (gBoard[length-1][width-1]) it, 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be gBoard[0][] or gBoard[][0]
						if (length>0 && width>0 && gBoard[length][width-1]>0 && gBoard[length-1][width-1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width-1)*10 + (length-1);
					}
					// Samurai
					else if (gBoard[length][width]==-3)
					{
						// Use a for loop to search for possible moves
						// Samurai can move directly forward until it runs into another piece or the end of the gBoard. 
						// Can only move horizontally to attack
						for (int forward = length+1; forward<BOARD_LENGTH; forward++)
						{
							if (gBoard[forward][width]==0)
							{
								gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10) + (forward);
							}else forward = BOARD_LENGTH; // Stop search because Samurai can't jump other pieces
						}
						// Now checking for horizontal moves to the left
						// Note that Samurai must not be in gBoard[][0]
						if (width>0)
						{
							for (int left = width-1; left>=0; left--)
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
						}
						// Now checking for horizontal moves to the right
						// Note that Samurai must not be in gBoard[][BOARD_WIDTH-1]
						if (width<BOARD_WIDTH-1)
						{
							for (int right = width + 1; right <BOARD_WIDTH; right++)
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
						for (int right = width + 1; right <BOARD_WIDTH && forward<BOARD_LENGTH; right++)
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
						for (int left = width - 1; left>=0 && forward<BOARD_LENGTH; left--)
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
						for (int right = width + 1; right < BOARD_WIDTH && back >= 0; right++)
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
						for (int left = width - 1; left>=0 && back >= 0; left--)
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
	public static boolean gameOver()
	{
		boolean result = false;
		// If there's a piece in front of the King, the game's over
		if (gBoard[6][3]!=0)
		{
			System.out.println("You win!!");
			result = true;
		}else if (gBoard[1][3]!=0)
		{
			System.out.println("Computer wins!!");
			result = true;
		}
		return result;
	}
	
	// Beginning of the minimax algorithm
	public static int miniMax()
	{
		int moveScore, move, oldLength, newLength, oldWidth, newWidth;
		int decrementFlag = 0;
        int bestMove = -1;
		int bestScore = -9999;
		for (int moveIndex = 0; moveIndex < gMIndex; move++)
		{
			// Get the move
			move = gMoves[moveIndex];
			// Decyphering 4 digit move
			oldLength = (move%1000)/100;(move/1000);
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
				gBoard[newLength-1][newWidth]++;
				decrementFlag++;
			}
			// Call min
			moveScore = min(1);
			// Determing the bestMove
			if (moveScore > bestScore)
			{
				bestScore = moveScore;
				bestMove = move;
			}
			// Retract move
			gBoard[oldLength][oldWidth] = gBoard[newLength][newWidth];
			gBoard[newLength][newWidth] = 0;
			if ((decrementFlag--)==1)
				gBoard[newLength+1][newWidth]--;
		}
		// Resetting the move index
		gMIndex = 0;
		return bestMove;
	}
	
	// Min sections tries to "predict" human move by finding the move that is best for them
	// Better moves for human are more negative
	public static int min(int depth)
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
			int bestScore = 9999;
			for (;humanIndex<humanStopIndex; humanIndex++)
			{
				// Get the move
				move = gMoves[humanIndex];
				// Decyphering 4 digit move
				oldLength = (move/1000);
				oldWidth = (move%1000)/100;
				newLength = (move%100)/10;
				newWidth = (move%10);
				// Make the actual move
				gBoard[newLength][newWidth] = gBoard[oldLength][oldWidth];
				gBoard[oldLength][oldWidth] = 0;
				// It's an attack move if the piece in front of where the move lands is an enemy
				// Decrement the piece, increment the attack flag
				if (gBoard[newLength+1][newWidth] > 0)
				{
					gBoard[newLength+1][newWidth]--;
					decrementFlag++;
				}
				moveScore = max(depth+1);
				if (moveScore<bestScore)
					bestScore = moveScore;
				// Retract move
				gBoard[oldLength][oldWidth] = gBoard[newLength][newWidth];
				gBoard[newLength][newWidth] = 0;
				if ((decrementFlag--)==1)
					gBoard[newLength+1][newWidth]++;
			}
			gMIndex = humanStartIndex;
			return bestScore;
		}
	}
	
	public static int max(int depth)
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
			int bestScore = 9999;
			for (;compIndex<compStopIndex; compIndex++)
			{
				// Get the move
				move = gMoves[compIndex];
				// Decyphering 4 digit move
				oldLength = (move/1000);
				oldWidth = (move%1000)/100;
				newLength = (move%100)/10;
				newWidth = (move%10);
				// Make the actual move
				gBoard[newLength][newWidth] = gBoard[oldLength][oldWidth];
				gBoard[oldLength][oldWidth] = 0;
				// It's an attack move if the piece in front of where the move lands is an enemy
				// Decrement the piece, increment the attack flag
				if (gBoard[newLength-1][newWidth] < 0)
				{
					gBoard[newLength-1][newWidth]++;
					decrementFlag++;
				}
				moveScore = max(depth+1);
				if (moveScore<bestScore)
					bestScore = moveScore;
				// Retract move
				gBoard[oldLength][oldWidth] = gBoard[newLength][newWidth];
				gBoard[newLength][newWidth] = 0;
				if ((decrementFlag--)==1)
					gBoard[newLength-1][newWidth]--;
			}
			return bestScore;
		}
	}
	
	
	public static int eval()
	{
		int result = 0;
		for (int length = 0; length < BOARD_LENGTH; length++)
		{
			for (int width = 0; width < BOARD_WIDTH; width++)
			{
				result += gBoard[length][width];
			}
		}
		return result;
	}
}