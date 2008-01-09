#	pragma once

#define C4_NONE      2
#define C4_MAX_LEVEL 20

/* A local struct which defines the state of a game. */

typedef struct {

	char **board;           /* The board configuration of the game state.  */
	/* board[x][y] specifies the position of the   */
	/* xth column and the yth row of the board,    */
	/* where column and row numbering starts at 0. */
	/* (The 0th row is the bottom row.)            */
	/* A value of 0 specifies that the position is */
	/* occupied by a piece owned by player 0, a    */
	/* value of 1 specifies that the position is   */
	/* occupied by a piece owned by player 1, and  */
	/* a value of C4_NONE specifies that the       */
	/* position is unoccupied.                     */

	int *(scoreArray[2]);  /* An array specifying statistics on both      */
	/* players.  score_array[0] specifies the      */
	/* statistics for player 0, while              */
	/* score_array[1] specifies the statistics for */
	/* player 1.                                   */

	int score[2];           /* The actual scores of each player, deducible */
	/* from score_array, but kept separately for   */
	/* efficiency.  The score of player x is the   */
	/* sum of score_array[x].  A score is          */
	/* basically a function of how many winning    */
	/* positions are still available to the        */
	/* and how close he/she is to achieving each   */
	/* of these positions.                         */

	short int winner;       /* The winner of the game - either 0, 1 or     */
	/* C4_NONE.  Deducible from score_array, but   */
	/* kept separately for efficiency.             */

	int numOfPieces;      /* The number of pieces currently occupying    */
	/* board spaces.  Deducible from board, but    */
	/* kept separately for efficiency.             */

} GAMESTATE;

class C4AI
{
public:
	C4AI();
	~C4AI();

	void newGame(int _width, int _height, int _num);
	bool makeMove(int _player, int _column, int *_row);
	int autoMove(int _player, int _level, int *_column, int *_row);
	char** board();
	int  scoreOfPlayer(int _player);
	bool isWinner(int _player);
	bool isTie();
	void winCoords(int *_x1, int *_y1, int *_x2, int *_y2);
	void endGame();
	void reset();

private:
	int sizeX, sizeY, numToConnect;
	int winPlaces;

	int ***map;  /* map[x][y] is an array of win place indices, */
	/* terminated by a -1.                         */

	int magicWinNumber;
	bool gameInProgress;
	bool moveInProgress;
	GAMESTATE stateStack[C4_MAX_LEVEL+1];
	GAMESTATE *currentState;
	int depth;
	int statesAllocated;
	int *dropOrder;

	/* A declaration of the local functions. */
	int numOfWinPlaces( int _x, int _y, int _n );
	void updateScore( int _player, int _x, int _y );
	int dropPiece( int _player, int _column );
	void pushState();
	int evaluate( int _player, int _level, int _alpha, int _beta );

};