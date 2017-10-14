import java.util.Scanner;
import java.lang.StringBuilder;

public class Shogun
{
	// Board size constants
	private static final int BOARD_LENGTH=8;
	private static final int BOARD_WIDTH=7;

	// Global variables
	private int[][] gBoard; // The board
	private int[] gMoves; // Array of possible moves
	private int gMIndex;
	
	public static void main(String[] args)
	{
		playGame();
	}

	public static void playGame()
	{
		Scanner kb = new Scanner(System.in);
		boolean play = true;
		boolean compTurn = false;
		boolean legal = false;
		int counter =0;
		System.out.println("Setting up the board...");
		gBoard = initBoard();
		System.out.print("Do you want to go first (y/n)? ");
		compTurn = kb.next().equalsIgnoreCase("y")?false:true;
		System.out.println();
		getMoves(gBoard, compTurn);
		System.out.println("Showing the board...");
		displayBoard(gBoard);
		/*while (false)
		{
			System.out.println("Getting legal moves...");
			if (compTurn)
			{	
				System.out.println("Comp making a turn...");
			}else
			{
				legal ^= false;
				while(!legal)
				{
					System.out.println("Player making a turn...");
					legal ^= true;
				}
				
			}
			System.out.println("Updating board state");
			System.out.println("Showing the board...");
			System.out.println("Checking if the game is over...");
			counter++;
		}*/
	}


	// Piece Definitions:
	//						King = K = +/-5
	//						Ninja = J = +/-4
	//						Samurai = S = +/-3
	//						Mini-Ninja = j = +/-2
	//						Mini-Samurai = s = +/-1
	public static void initBoard()
	{
		gBoard = {{0, 0, 0, -5, 0, 0, 0},
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
	public static void getMoves(int[][] board, boolean compTurn)
	{
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
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10-1) + (length);
						// If there is an enemy piece to the right (gBoard[][width+1]) AND forward (gBoard[length-1][]) 
						// of the Mini-Samurai AND there is no piece directly to the right(gBoard[length][width+1])...
						// Mini-Samurai must not be in gBoard[][BOARD_WIDTH-1]
						if (width<BOARD_WIDTH-1 && gBoard[length][width+1]==0 && gBoard[length-1][width+1]<0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10+1) + (length);
					}
					// Mini-Ninja
					else if (gBoard[length][width]==2)
					{
						// If there is no piece diagonally forward to the right (gBoard[length-1][width+1]) of Mini-Ninja...
						// Mini-Ninja must not be gBoard[0][] or gBoard[][BOARD_WIDTH-1]
						if (length>0 && width<BOARD_WIDTH-1 && gBoard[length-1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10+1) + (length-1);
						// If there is no piece diagonally forward to the left (gBoard[length-1][width-1]) of Mini-Ninja...
						// Mini-Ninja must not be gBoard[0][] or gBoard[][0]
						if (length>0 && width>0 && gBoard[length-1][width-1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10-1) + (length-1);
						// If there is an enemy piece to the right (gBoard[][width+1]) with no piece behind it(gBoard[length+1][width+1]), 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][BOARD_WIDTH-1]
						if (length<BOARD_LENGTH-1 && width<BOARD_WIDTH-1 && gBoard[length][width+1]<0 && gBoard[length+1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10+1) + (length+1);
						// If there is an enemy piece to the left (gBoard[][width-1]) with no piece behind it (gBoard[length+1][width-1]), 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][0]
						if (length<BOARD_LENGTH-1 && width>0 && gBoard[length][width-1]<0 && gBoard[length+1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10-1) + (length+1);
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
								}else right = -1; // If the square to the right is blocked, there is no need to check further
							}
						}
					}
					// Ninja
					else if(gBoard[length][width]==4)
					{
						for (int forward = length-1; forward>=0; forward--)
						{
							for (int right = width + 1; right <BOARD_WIDTH; right++)
							{
								if (gBoard[forward][right]==0)
								{
									gMoves[gMIndex++] = (width*1000) + (length*100) + (right*10) + (forward);
								}else 
								{
									forward = -1; // Stop search because Ninja can't jump other pieces
									right = BOARD_WIDTH;
								}
							}
						}
						for (int forward = length-1; forward>=0; forward--)
						{
							for (int left = width - 1; left>=0; left--)
							{
								if (gBoard[forward][left]==0)
								{
									gMoves[gMIndex++] = (width*1000) + (length*100) + (left*10) + (forward);
								}else
								{
									forward = -1; // Stop search because Ninja can't jump other pieces
									left = -1;
								}

						}
						for (int back = length + 1; back < BOARD_LENGTH; back++)
						{
							for (int right = width + 1; right < BOARD_WIDTH; right++)
							{
								if (gBoard[back][right]==0) 
								{
									if (gBoard[back-1][right]<0)
									{
										gMoves[gMIndex++] = (width*1000) + (length*100) + (right*10) + (back);
									}
								}else 
								{	
									right = BOARD_WIDTH;
									back = BOARD_LENGTH;
								}
							}
						}
						for (int back = length + 1; back < BOARD_LENGTH; back++)
						{
							for (int left = width - 1; left>=0; left--)
							{
								if (gBoard[back][left]==0 
								{
									if (gBoard[back-1][left]<0)
									{
										gMoves[gMIndex++] = (width*1000) + (length*100) + (left*10) + (back);
									}
								}else 
								{
									left = -1;
									back = BOARD_LENGTH;
								}
							}
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
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10-1) + (length);
						// If there is an enemy piece to the right AND forward (gBoard[length+1][width+1]) of the Mini-Samurai 
						// AND there is no piece directly to the right (gBoard[length][width+1])...
						// Mini-Samurai must not be in gBoard[][BOARD_WIDTH-1]
						if (width<BOARD_WIDTH-1 && gBoard[length][width+1]==0 && gBoard[length+1][width+1]>0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10+1) + (length);
					}
					// Mini-Ninja
					else if (gBoard[length][width]==-2)
					{
						// If there is no piece diagonally forward to the right (gBoard[length+1][width+1]) of Mini-Ninja...
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][BOARD_WIDTH-1]
						if (length<BOARD_LENGTH-1 && width<BOARD_WIDTH-1 && gBoard[length+1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10+1) + (length+1);
						// If there is no piece diagonally forward to the left (gBoard[length+1][width-1]) of Mini-Ninja...
						// Mini-Ninja must not be gBoard[BOARD_LENGTH-1][] or gBoard[][0]
						if (length<BOARD_LENGTH-1 && width>0 && gBoard[length+1][width-1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10-1) + (length+1);
						// If there is an enemy piece to the right (gBoard[length][width+1]) with no piece behind it (gBoard[length-1][width+1]), 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be gBoard[0][] or gBoard[][BOARD_WIDTH-1]
						if (length>0 && width<BOARD_WIDTH-1 && gBoard[length][width+1]<0 && gBoard[length-1][width+1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10+1) + (length-1);
						// If there is an enemy piece to the left (gBoard[length][width-1]) with no piece behind (gBoard[length-1][width-1]) it, 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be gBoard[0][] or gBoard[][0]
						if (length>0 && width>0 && gBoard[length][width-1]<0 && gBoard[length-1][width-1]==0)
							gMoves[gMIndex++] = (width*1000) + (length*100) + (width*10-1) + (length-1);
					}
				}
			}
		}
		System.out.println(moves);
	}
}