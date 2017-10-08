import java.util.Scanner;
import java.lang.StringBuilder;

public class Shogun
{
	private static final int BOARD_LENGTH=8;
	private static final int BOARD_WIDTH=7;

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
		int[][] board = initBoard();
		System.out.print("Do you want to go first (y/n)? ");
		compTurn = kb.next().equalsIgnoreCase("y")?false:true;
		System.out.println();
		getMoves(board, compTurn);
		System.out.println("Showing the board...");
		displayBoard(board);
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
	public static int[][] initBoard()
	{
		int[][] board = {{0, 0, 0, -5, 0, 0, 0},
						 {-3, -3, -3, 0, -4, -4, -4},
						 {-2, -2, -2, 0, -1, -1, -1},
						 {0, 0, 0, 0, 0, 0, 0},
						 {0, 0, 0, 0, 0, 0, 0},
						 {1, 1, 1, 0, 2, 2, 2},
						 {4, 4, 4, 0, 3, 3, 3},
						 {0, 0, 0, 5, 0, 0, 0}};
		return board;
	}

	public static void displayBoard(int[][] board)
	{
		for (int length = BOARD_LENGTH-1; length>=0; length--)
		{
			System.out.print(length + 1);
			for (int width = 0; width<BOARD_WIDTH; width++)
			{
				System.out.printf("%3d", board[length][width]);
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
		StringBuilder moves = new StringBuilder();
		// If it's the computer's turn, find possible moves for the computer
		// All moves described from the computer's POV
		// Computer moves from board[7][] toward board[0][]
		// Left for the computer is board[][0] and right is board[][7]
		if (compTurn)
		{
			for (int length = 0; length < BOARD_LENGTH; length++)
			{
				for (int width = 0; width < BOARD_WIDTH; width++)
				{
					// Mini-Samurai
					if (board[length][width]==16)
					{
						// If there is no piece in front (board[length-1][]) of the Mini-Samurai, it's possible to move forward one space
						// Mini-Samurai must also not be in board[0][] row
						if (length>0 && board[length-1][width]==0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width+'A')).append(length+" ");
						// If there is an enemy piece to the left (board[][width-1]) AND forward (board[length-1][]) 
						// of the Mini-Samurai AND there is no piece directly to the left (board[length][width-1])...
						// Mini-Samurai must not be in board[][0]
						if (width>0 && board[length][width-1]==0 && board[length-1][width-1]<0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width-1+'A')).append(length+1+" ");
						// If there is an enemy piece to the right (board[][width+1]) AND forward (board[length-1][]) 
						// of the Mini-Samurai AND there is no piece directly to the right(board[length][width+1])...
						// Mini-Samurai must not be in board[][BOARD_WIDTH-1]
						if (width<BOARD_WIDTH-1 && board[length][width+1]==0 && board[length-1][width+1]<0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width+1+'A')).append(length+1+" ");
					}
					// Mini-Ninja
					else if (board[length][width]==2)
					{
						// If there is no piece diagonally forward to the right (board[length-1][width+1]) of Mini-Ninja...
						// Mini-Ninja must not be board[0][] or board[][BOARD_WIDTH-1]
						if (length>0 && width<BOARD_WIDTH-1 && board[length-1][width+1]==0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width+1+'A')).append(length+" ");
						// If there is no piece diagonally forward to the left (board[length-1][width-1]) of Mini-Ninja...
						// Mini-Ninja must not be board[0][] or board[][0]
						if (length>0 && width>0 && board[length-1][width-1]==0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width-1+'A')).append(length+" ");
						// If there is an enemy piece to the right (board[][width+1]) with no piece behind it(board[length+1][width+1]), 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be board[BOARD_LENGTH-1][] or board[][BOARD_WIDTH-1]
						if (length<BOARD_LENGTH-1 && width<BOARD_WIDTH-1 && board[length][width+1]<0 && board[length+1][width+1]==0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width+1+'A')).append(length+2+" ");
						// If there is an enemy piece to the left (board[][width-1]) with no piece behind it (board[length+1][width-1]), 
						// Mini-Ninja can move there to attack
						// Mini-Ninja must not be board[BOARD_LENGTH-1][] or board[][0]
						if (length<BOARD_LENGTH-1 && width>0 && board[length][width-1]<0 && board[length+1][width+1]==0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width-1+'A')).append(length+2+" ");
					}
				}
			}
		}
		// Finding the possible moves for the human player
		// All moves described from the player's POV
		else
		{
			for (int length = 0; length < BOARD_LENGTH; length++)
			{
				for (int width = 0; width < BOARD_WIDTH; width++)
				{
					// Mini-Samurai
					if (board[length][width]==-1)
					{
						// If there is no piece in front of the Mini-Samurai, it's possible to move forward one space
						// Piece must also not be all the way at the end of the board
						if (length>0 && board[length+1][width]==0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width+'A')).append(length+2+" ");
						// If there is an enemy piece to the left AND forward of the Mini-Samurai AND there is no piece directly to the left...
						// Width must be greater than 0
						if (width>0 && board[length][width-1]==0 && board[length+1][width-1]>0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width-1+'A')).append(length+1+" ");
						// If there is an enemy piece to the right AND forward of the Mini-Samurai AND there is no piece directly to the right...
						// Width must be less than BOARD_WIDTH-1
						if (width<BOARD_WIDTH-1 && board[length][width+1]==0 && board[length+1][width+1]>0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width+1+'A')).append(length+1+" ");
					}
					// Mini-Ninja
					else if (board[length][width]==-2)
					{
						// If there is no piece diagonally forward to the right of Mini-Ninja...
						// Piece must be one away from the right side of the board and not all the way at the end of the board
						if (length>0 && width<BOARD_WIDTH-1 && board[length-1][width+1]==0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width+1+'A')).append(length+" ");
						// If there is no piece diagonally forward to the left of Mini-Ninja...
						// Piece must be one away from the left side of the board and not all the way to the end of the board
						if (length>0 && width>0 && board[length-1][width-1]==0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width-1+'A')).append(length+" ");
						// If there is an enemy piece to the right with no piece behind it, Mini-Ninja can move there to attack
						// Mini-Ninja must be one away from right side of the board and must be one away from the end of the board
						if (length<BOARD_LENGTH-1 && width<BOARD_WIDTH-1 && board[length][width+1]<0 && board[length+1][width+1]==0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width+1+'A')).append(length+2+" ");
						// If there is an enemy piece to the left with no piece behind it, Mini-Ninja can move there to attack
						if (length<BOARD_LENGTH-1 && width>0 && board[length][width-1]<0 && board[length+1][width+1]==0)
							moves.append((char)(width+'A')).append(length+1).append((char)(width-1+'A')).append(length+2+" ");
					}
				}
			}
		}
		System.out.println(moves);
	}
}