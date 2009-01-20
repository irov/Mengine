
#	include "C4AI.h"

#	include <memory>

#define other(x)       ((x) ^ 1)
#define realPlayer(x)  ((x) & 1)

#define popState() \
	(currentState = &stateStack[--depth])

#define INT_MAX 2147483647
/* The "goodness" of the current state with respect to a player is the */
/* score of that player minus the score of the player's opponent.  A   */
/* positive value will result if the specified player is in a better   */
/* situation than his/her opponent.                                    */

#define goodnessOf( player ) \
	(currentState->score[player] - currentState->score[ other( player ) ] )

C4AI::C4AI()
: gameInProgress( false )
, moveInProgress( false )
, statesAllocated( 0 )
{

}

C4AI::~C4AI()
{

}

void C4AI::newGame(int _width, int _height, int _num)
{
	int i, j, k, x;
	int winIndex, column;
	int *winIndices;

	//assert(!game_in_progress);
	//assert(width >= 1 && height >= 1 && num >= 1);

	sizeX = _width;
	sizeY = _height;
	numToConnect = _num;
	magicWinNumber = 1 << numToConnect;
	winPlaces = numOfWinPlaces( sizeX, sizeY, numToConnect );

	/* Set up the board */
	depth = 0;
	currentState = &stateStack[0];

	currentState->board = (char **) malloc(sizeX * sizeof(char *));
	for ( i = 0; i < sizeX; i++ ) 
	{
		currentState->board[i] = (char *) malloc(sizeY);
		for ( j = 0; j < sizeY; j++ )
		{
			currentState->board[i][j] = C4_NONE;
		}
	}

	/* Set up the score array */

	currentState->scoreArray[0] = (int *) malloc(winPlaces * sizeof(int));
	currentState->scoreArray[1] = (int *) malloc(winPlaces * sizeof(int));
	for ( i = 0; i < winPlaces; i++ ) 
	{
		currentState->scoreArray[0][i] = 1;
		currentState->scoreArray[1][i] = 1;
	}

	currentState->score[0] = currentState->score[1] = winPlaces;
	currentState->winner = C4_NONE;
	currentState->numOfPieces = 0;

	statesAllocated = 1;

	/* Set up the map */

	map = (int ***) malloc(sizeX * sizeof(int **));
	for ( i = 0; i < sizeX; i++ ) 
	{
		map[i] = (int **) malloc(sizeY * sizeof(int *));
		for ( j = 0; j < sizeY; j++ ) 
		{
			map[i][j] = (int *) malloc((numToConnect * 4 + 1) * sizeof(int));
			map[i][j][0] = -1;
		}
	}

	winIndex = 0;

	/* Fill in the horizontal win positions */
	for ( i = 0; i<sizeY; i++ )
	{
		for (j=0; j<sizeX - numToConnect + 1; j++) 
		{
			for ( k = 0; k < numToConnect; k++ ) 
			{
				winIndices = map[j+k][i];
				for ( x = 0; winIndices[x] != -1; x++ )
					;
				winIndices[x++] = winIndex;
				winIndices[x] = -1;
			}
			winIndex++;
		}
	}

	/* Fill in the vertical win positions */
	for ( i = 0; i < sizeX; i++ )
	{
		for ( j = 0; j < sizeY - numToConnect + 1; j++ )
		{
			for ( k = 0; k < numToConnect; k++ ) 
			{
				winIndices = map[i][j+k];
				for (x=0; winIndices[x] != -1; x++)
					;
				winIndices[x++] = winIndex;
				winIndices[x] = -1;
			}
			winIndex++;
		}
	}

	/* Fill in the forward diagonal win positions */
	for ( i = 0; i < sizeY - numToConnect + 1; i++ )
	{
		for ( j = 0; j < sizeX - numToConnect + 1; j++ ) 
		{
			for ( k = 0; k < numToConnect; k++ ) 
			{
				winIndices = map[j+k][i+k];
				for ( x = 0; winIndices[x] != -1; x++ )
					;
				winIndices[x++] = winIndex;
				winIndices[x] = -1;
			}
			winIndex++;
		}
	}

	/* Fill in the backward diagonal win positions */
	for ( i = 0; i < sizeY - numToConnect + 1; i++ )
	{
		for ( j = sizeX - 1; j >= numToConnect - 1; j--) 
		{
			for ( k = 0; k < numToConnect; k++ ) 
			{
				winIndices = map[j-k][i+k];
				for ( x = 0; winIndices[x] != -1; x++ )
					;
				winIndices[x++] = winIndex;
				winIndices[x] = -1;
			}
			winIndex++;
		}
	}

	/* Set up the order in which automatic moves should be tried. */
	/* The columns nearer to the center of the board are usually  */
	/* better tactically and are more likely to lead to a win.    */
	/* By ordering the search such that the central columns are   */
	/* tried first, alpha-beta cutoff is much more effective.     */

	dropOrder = (int *) malloc(sizeX * sizeof(int));
	column = ( sizeX - 1 ) / 2;
	for ( i = 1; i <= sizeX; i++ ) 
	{
		dropOrder[i - 1] = column;
		column += ( (i % 2)? i : -i);
	}

	gameInProgress = true;
}


/****************************************************************************/
/**                                                                        **/
/**  This function drops a piece of the specified player into the          **/
/**  specified column.  A value of TRUE is returned if the drop is         **/
/**  successful, or FALSE otherwise.  A drop is unsuccessful if the        **/
/**  specified column number is invalid or full.  If the drop is           **/
/**  successful and row is a non-NULL pointer, the row where the piece     **/
/**  ended up is returned through the row pointer.  Note that column and   **/
/**  row numbering start at 0.                                             **/
/**                                                                        **/
/****************************************************************************/

bool C4AI::makeMove(int player, int column, int *row)
{
	int result; 

	//assert(game_in_progress);
	//assert(!move_in_progress);

	if (column >= sizeX || column < 0)
		return false;

	result = dropPiece( realPlayer( player ), column );
	if ( row && result >= 0 )
		*row = result;
	return ( result >= 0 );
}


/****************************************************************************/
/**                                                                        **/
/**  This function instructs the computer to make a move for the specified **/
/**  player.  level specifies the number of levels deep the computer       **/
/**  should search the game tree in order to make its decision.  This      **/
/**  corresponds to the number of "moves" in the game, where each player's **/
/**  turn is considered a move.  A value of TRUE is returned if a move was **/
/**  made, or FALSE otherwise (i.e. if the board is full).  If a move was  **/
/**  made, the column and row where the piece ended up is returned through **/
/**  the column and row pointers (unless a pointer is NULL, in which case  **/
/**  it won't be used to return any information).  Note that column and    **/
/**  row numbering start at 0.  Also note that for a standard 7x6 game of  **/
/**  Connect-4, the computer is brain-dead at levels of three or less,     **/
/**  while at levels of four or more the computer provides a challenge.    **/
/**                                                                        **/
/****************************************************************************/

int C4AI::autoMove( int _player, int _level, int *_column, int *_row )
{
	int i;
	int bestColumn = -1;
	int goodness = 0;
	int bestWorst = -(INT_MAX);
	int numOfEqual = 0;
	int realPlayer;
	int currentColumn;
	int result;

	//assert(game_in_progress);
	//assert(!move_in_progress);
	//assert(level >= 1 && level <= C4_MAX_LEVEL);

	realPlayer = realPlayer( _player );

	/* It has been proven that the best first move for a standard 7x6 game  */
	/* of connect-4 is the center column.  See Victor Allis' masters thesis */
	/* ("ftp://ftp.cs.vu.nl/pub/victor/connect4.ps") for this proof.        */

	if ( currentState->numOfPieces < 2 && sizeX == 7 && sizeY == 6 && numToConnect == 4 &&
		( currentState->numOfPieces == 0 || currentState->board[3][0] != C4_NONE ) ) 
	{
			if ( _column )
				*_column = 3;
			if ( _row )
				*_row = currentState->numOfPieces;
			dropPiece( realPlayer, 3 );
			return 3;
	}

	moveInProgress = true;

	/* Simulate a drop in each of the columns and see what the results are. */

	for ( i = 0; i < sizeX; i++ )
	{
		pushState();
		currentColumn = dropOrder[i];

		/* If this column is full, ignore it as a possibility. */
		if ( dropPiece( realPlayer, currentColumn ) < 0 ) 
		{
			popState();
			continue;
		}

		/* If this drop wins the game, take it! */
		else if ( currentState->winner == realPlayer ) 
		{
			bestColumn = currentColumn;
			popState();
			break;
		}

		/* Otherwise, look ahead to see how good this move may turn out */
		/* to be (assuming the opponent makes the best moves possible). */
		else 
		{
			goodness = evaluate( realPlayer, _level, -(INT_MAX), -bestWorst );
		}

		/* If this move looks better than the ones previously considered, */
		/* remember it.                                                   */
		if (goodness > bestWorst) 
		{
			bestWorst = goodness;
			bestColumn = currentColumn;
			numOfEqual = 1;
		}

		/* If two moves are equally as good, make a random decision. */
		else if (goodness == bestWorst) 
		{
			numOfEqual++;
			if ( rand() % 10000 < ((float)1 / (float)numOfEqual) * 10000 )
				bestColumn = currentColumn;
		}

		popState();
	}

	moveInProgress = false;

	/* Drop the piece in the column decided upon. */

	if ( bestColumn >= 0 ) 
	{
		result = dropPiece( realPlayer, bestColumn );
		if ( _column )
		{
			*_column = bestColumn;
		}
		if ( _row )
		{
			*_row = result;
		}
		return bestColumn;
	}
	else
		return 0;
}


/****************************************************************************/
/**                                                                        **/
/**  This function returns a two-dimensional array containing the state of **/
/**  the game board.  Do not modify this array.  It is assumed that a game **/
/**  is in progress.  The value of this array is dynamic and will change   **/
/**  to reflect the state of the game as the game progresses.  It becomes  **/
/**  and stays undefined when the game ends.                               **/
/**                                                                        **/
/**  The first dimension specifies the column number and the second        **/
/**  dimension specifies the row number, where column and row numbering    **/
/**  start at 0 and the bottow row is considered the 0th row.  A value of  **/
/**  0 specifies that the position is occupied by a piece owned by player  **/
/**  0, a value of 1 specifies that the position is occupied by a piece    **/
/**  owned by player 1, and a value of C4_NONE specifies that the position **/
/**  is unoccupied.                                                        **/
/**                                                                        **/
/****************************************************************************/

char ** C4AI::board()
{
	return currentState->board;
}


/****************************************************************************/
/**                                                                        **/
/**  This function returns the "score" of the specified player.  This      **/
/**  score is a function of how many winning positions are still available **/
/**  to the player and how close he/she is to achieving each of these      **/
/**  positions.  The scores of both players can be compared to observe how **/
/**  well they are doing relative to each other.                           **/
/**                                                                        **/
/****************************************************************************/

int C4AI::scoreOfPlayer( int _player )
{
	return currentState->score[realPlayer( _player )];
}


/****************************************************************************/
/**                                                                        **/
/**  This function returns TRUE if the specified player has won the game,  **/
/**  and FALSE otherwise.                                                  **/
/**                                                                        **/
/****************************************************************************/

bool C4AI::isWinner( int _player )
{
	return ( currentState->winner == realPlayer( _player ) );
}


/****************************************************************************/
/**                                                                        **/
/**  This function returns TRUE if the board is completely full, and FALSE **/
/**  otherwise.                                                            **/
/**                                                                        **/
/****************************************************************************/

bool C4AI::isTie()
{
	return ( currentState->numOfPieces == sizeX * sizeY );
}


/****************************************************************************/
/**                                                                        **/
/**  This function returns the coordinates of the winning connections of   **/
/**  the winning player.  It is assumed that a player has indeed won the   **/
/**  game.  The coordinates are returned in x1, y1, x2, y2, where (x1, y1) **/
/**  specifies the lower-left piece of the winning connection, and         **/
/**  (x2, y2) specifies the upper-right piece of the winning connection.   **/
/**  If more than one winning connection exists, only one will be          **/
/**  returned.                                                             **/
/**                                                                        **/
/****************************************************************************/

void C4AI::winCoords(int *_x1, int *_y1, int *_x2, int *_y2)
{
	int i, j, k;
	int winner;
	int winPos = 0;
	bool found;

	winner = currentState->winner;
	
	while ( currentState->scoreArray[winner][winPos] != magicWinNumber )
		winPos++;

	/* Find the lower-left piece of the winning connection. */

	found = false;
	for ( j = 0; j < sizeY && !found; j++ )
	{
		for ( i = 0; i < sizeX; i++ )
		{
			for ( k = 0; map[i][j][k] != -1; k++ )
			{
				if (map[i][j][k] == winPos) 
				{
					*_x1 = i;
					*_y1 = j;
					found = true;
					break;
				}
			}
		}
	}

	/* Find the upper-right piece of the winning connection. */

	found = false;
	for ( j = sizeY - 1; j >= 0 && !found; j-- )
	{
		for ( i = sizeX - 1; i >= 0; i-- )
		{
			for (k=0; map[i][j][k] != -1; k++)
			{
				if ( map[i][j][k] == winPos ) 
				{
					*_x2 = i;
					*_y2 = j;
					found = true;
					break;
				}
			}
		}
	}
}


/****************************************************************************/
/**                                                                        **/
/**  This function ends the current game.  It is assumed that a game is    **/
/**  in progress.  It is illegal to call any other game function           **/
/**  immediately after this one except for c4_new_game(), c4_poll() and    **/
/**  c4_reset().                                                           **/
/**                                                                        **/
/****************************************************************************/

void C4AI::endGame()
{
	int i, j;

	/* Free up the memory used by the map. */
	for ( i = 0; i < sizeX; i++ ) 
	{
		for ( j = 0; j < sizeY; j++ )
		{
			free( map[i][j] );
		}
		free( map[i] );
	}
	free( map );

	/* Free up the memory of all the states used. */

	for ( i = 0; i < statesAllocated; i++ ) 
	{
		for ( j = 0; j < sizeX; j++ )
		{
			free( stateStack[i].board[j] );
		}
		free( stateStack[i].board );
		free( stateStack[i].scoreArray[0] );
		free( stateStack[i].scoreArray[1] );
	}
	statesAllocated = 0;

	/* Free up the memory used by the drop_order array. */

	free( dropOrder );

	gameInProgress = false;
}


/****************************************************************************/
/**                                                                        **/
/**  This function resets the state of the algorithm to the starting state **/
/**  (i.e., no game in progress and a NULL poll function).  There should   **/
/**  no reason to call this function unless for some reason the calling    **/
/**  algorithm loses track of the game state.  It is illegal to call any   **/
/**  other game function immediately after this one except for             **/
/**  c4_new_game(), c4_poll() and c4_reset().                              **/
/**                                                                        **/
/****************************************************************************/

void C4AI::reset()
{
	if ( gameInProgress )
		endGame();
}

/****************************************************************************/
/**                                                                        **/
/**  This function returns the number of possible win positions on a board **/
/**  of dimensions x by y with n being the number of pieces required in a  **/
/**  row in order to win.                                                  **/
/**                                                                        **/
/****************************************************************************/

int C4AI::numOfWinPlaces(int _x, int _y, int _n)
{
	if ( _x < _n && _y < _n )
		return 0;
	else if ( _x < _n )
		return _x * ( ( _y - _n ) + 1 );
	else if ( _y < _n )
		return _y * ( ( _x - _n ) + 1 );
	else
		return 4*_x*_y - 3*_x*_n - 3*_y*_n + 3*_x + 3*_y - 4*_n + 2*_n*_n + 2;
}


/****************************************************************************/
/**                                                                        **/
/**  This function updates the score of the specified player in the        **/
/**  context of the current state,  given that the player has just placed  **/
/**  a game piece in column x, row y.                                      **/
/**                                                                        **/
/****************************************************************************/

void C4AI::updateScore( int _player, int _x, int _y )
{
	int i;
	int winIndex;
	int thisDifference = 0, otherDifference = 0;
	int **currentScoreArray = currentState->scoreArray;
	int otherPlayer = other( _player );

	for ( i = 0; map[_x][_y][i] != -1; i++ ) 
	{
		winIndex = map[_x][_y][i];
		thisDifference += currentScoreArray[_player][winIndex];
		otherDifference += currentScoreArray[otherPlayer][winIndex];

		currentScoreArray[_player][winIndex] <<= 1;
		currentScoreArray[otherPlayer][winIndex] = 0;

		if (currentScoreArray[_player][winIndex] == magicWinNumber)
		{
			if (currentState->winner == C4_NONE)
			{
				currentState->winner = _player;
			}
		}
	}

	currentState->score[_player] += thisDifference;
	currentState->score[otherPlayer] -= otherDifference;
}


/****************************************************************************/
/**                                                                        **/
/**  This function drops a piece of the specified player into the          **/
/**  specified column.  The row where the piece ended up is returned, or   **/
/**  -1 if the drop was unsuccessful (i.e., the specified column is full). **/
/**                                                                        **/
/****************************************************************************/

int C4AI::dropPiece( int _player, int _column )
{
	int y = 0;

	while ( currentState->board[_column][y] != C4_NONE && ++y < sizeY )
		;

	if (y == sizeY)
		return -1;

	currentState->board[_column][y] = _player;
	currentState->numOfPieces++;
	updateScore( _player, _column, y);

	return y;
}


/****************************************************************************/
/**                                                                        **/
/**  This function pushes the current state onto a stack.  popdir() is     **/
/**  used to pop from this stack.                                          **/
/**                                                                        **/
/**  Technically what it does, since the current state is considered to    **/
/**  be the top of the stack, is push a copy of the current state onto     **/
/**  the stack right above it.  The stack pointer (depth) is then          **/
/**  incremented so that the new copy is considered to be the current      **/
/**  state.  That way, all pop_state() has to do is decrement the stack    **/
/**  pointer.                                                              **/
/**                                                                        **/
/**  For efficiency, memory for each stack state used is only allocated    **/
/**  once per game, and reused for the remainder of the game.              **/
/**                                                                        **/
/****************************************************************************/

void C4AI::pushState()
{
	int i;
	int winPlacesArraySize;
	GAMESTATE *oldState, *newState;

	winPlacesArraySize = winPlaces * sizeof(int);
	oldState = &stateStack[depth++];
	newState = &stateStack[depth];

	if (depth == statesAllocated) 
	{

		/* Allocate space for the board */

		newState->board = (char **) malloc(sizeX * sizeof(char *));
		for ( i = 0; i < sizeX; i++ )
		{
			newState->board[i] = (char *) malloc(sizeY);
		}

		/* Allocate space for the score array */

		newState->scoreArray[0] = (int *) malloc(winPlacesArraySize);
		newState->scoreArray[1] = (int *) malloc(winPlacesArraySize);

		statesAllocated++;
	}

	/* Copy the board */

	for ( i = 0; i < sizeX; i++ )
	{
		memcpy( newState->board[i], oldState->board[i], sizeY );
	}

	/* Copy the score array */

	memcpy( newState->scoreArray[0], oldState->scoreArray[0], winPlacesArraySize );
	memcpy( newState->scoreArray[1], oldState->scoreArray[1], winPlacesArraySize );

	newState->score[0] = oldState->score[0];
	newState->score[1] = oldState->score[1];
	newState->winner = oldState->winner;

	currentState = newState;
}


/****************************************************************************/
/**                                                                        **/
/**  This recursive function determines how good the current state may     **/
/**  turn out to be for the specified player.  It does this by looking     **/
/**  ahead level moves.  It is assumed that both the specified player and  **/
/**  the opponent may make the best move possible.  alpha and beta are     **/
/**  used for alpha-beta cutoff so that the game tree can be pruned to     **/
/**  avoid searching unneccessary paths.                                   **/
/**                                                                        **/
/**  The specified poll function (if any) is called at the appropriate     **/
/**  intervals.                                                            **/
/**                                                                        **/
/**  The worst goodness that the current state can produce in the number   **/
/**  of moves (levels) searched is returned.  This is the best the         **/
/**  specified player can hope to achieve with this state (since it is     **/
/**  assumed that the opponent will make the best moves possible).         **/
/**                                                                        **/
/****************************************************************************/

int C4AI::evaluate( int _player, int _level, int _alpha, int _beta )
{
	int i, goodness, best, maxab;

	if ( _level == depth )
	{
		return goodnessOf( _player );
	}
	else 
	{
		/* Assume it is the other player's turn. */
		best = -(INT_MAX);
		maxab = _alpha;
		for( i = 0; i < sizeX; i++ ) 
		{
			pushState();
			if ( dropPiece( other( _player ), dropOrder[i]) < 0) 
			{
				popState();
				continue;
			}
			else if ( currentState->winner == other( _player ) )
			{
				goodness = INT_MAX - depth;
			}
			else
			{
				goodness = evaluate( other( _player ), _level, -_beta, -maxab);
			}
			if ( goodness > best )
			{
				best = goodness;
				if (best > maxab)
					maxab = best;
			}
			popState();
			if ( best > _beta )
			{
				break;
			}
		}

		/* What's good for the other player is bad for this one. */
		return -best;
	}
}
