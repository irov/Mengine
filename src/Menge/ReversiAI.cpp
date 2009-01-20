#	include "ReversiAI.h"

#	include "stdio.h"

ReversiAI::ReversiAI()
{
	//set default parameters
	meId = 1;
	youId = -1; 
	emptyId = 0;
	depth = 8;
	egEmpty = 12;
	moveOrderDepth = 2;

	scoreWin = 100000;
	scoreLoss = -100000;
	scoreDraw = 0;
	startAlpha = -999999; 
	startBeta = 999999;

	//some limiting values
	maxDepth = 35;
	maxDisksTurned = 20;
	maxLegalMoves = 40;

	// allocate memory for move structures
	usedMoveStruc = 0;
	startMoveStruc = new char[ maxDepth * maxDisksTurned * maxLegalMoves];
	// space for move structures
	// 35-max depth; 20-max disks overturned by a move
	// 40-max moves possible in a position

	//set the order of search of depth 2 to be logical
	order[0] = 0; order[1] = 7; order[2] = 56; order[3] = 63;
	order[4] = 2; order[5] = 3; order[6] = 4; order[7] = 5;
	order[8] = 23; order[9] = 31; order[10] = 39; order[11] = 47;
	order[12] = 16; order[13] = 24; order[14] = 32; order[15] = 40;
	order[16] = 58; order[17] = 59; order[18] = 60; order[19] = 61;
	order[20] = 18; order[21] = 21; order[22] = 42; order[23] = 45;
	order[24] = 43; order[25] = 37; order[26] = 20; order[27] = 26;
	order[28] = 34; order[29] = 44; order[30] = 29; order[31] = 19;
	order[32] = 10; order[33] = 11; order[34] = 12; order[35] = 13;
	order[36] = 22; order[37] = 30; order[38] = 38; order[39] = 46;
	order[40] = 17; order[41] = 25; order[42] = 33; order[43] = 41;
	order[44] = 50; order[45] = 51; order[46] = 52; order[47] = 53;
	order[48] = 48; order[49] = 55; order[50] = 15; order[51] = 8;
	order[52] = 57; order[53] = 62; order[54] = 6; order[55] = 1;
	order[56] = 49; order[57] = 54; order[58] = 14; order[59] = 9;
}

ReversiAI::~ReversiAI()
{
	delete startMoveStruc;
}

void ReversiAI::setId( int _me, int _you, int _empty )
{ 
	meId = _me;
	youId = _you;
	emptyId = _empty; 
}

void ReversiAI::setDepth( int _d )
{ 
	depth = _d; 
}

void ReversiAI::setEndgameEmpty( int _d )
{
	egEmpty = _d; 
}

void ReversiAI::markCell( int _playerID, int _x, int _y )
{
	int mark = 0;
	if( _playerID == meId )
		mark = 1;
	else if( _playerID == youId )
		mark = -1;

	board[ _y * 8 + _x ] = mark;
}

int ReversiAI::autoMove()
{
	for( int i = 0;  i < 8; i++ )
	{
		for( int j = 0; j < 8; j++ )
			printf( "%d  ", board[ (7 - i)*8 + j]);
		printf("\n");
	}
	count = 0; //the no of leaves evaluated
	//copy the given board into a local array & simultaneously find the total no of disks
	totalDisks = 64;
	for ( int i = 0; i <= 63; i++ )
	{
		if ( board[i] == 0 ) 
		{ 
			totalDisks--; 
		}
	}

	toPlay = 1;  //me, myself
	int alpha = startAlpha;
	int beta = startBeta;
	// Important(the following line): if we have entered the end game, then
	// make depth infinite(60), to search till the end of the game...
	int depthThisTime = ( totalDisks > 64 - egEmpty )? 60 : depth;

	moveOrderSearch = false;
	//initially this is true search; not the shallow search initiated by order_moves()
	// to perform move ordering.

	char* moveStruc;
	moveStruc = allocMoveStruc();


	int numMoves = setUpLegalMoves( moveStruc );

	if ( numMoves == 0 )  //no moves to play for us!
	{
		char* tempMoveStruc;
		int q;

		tempMoveStruc = allocMoveStruc();
		toPlay = -toPlay;
		q = setUpLegalMoves( tempMoveStruc );
		toPlay = -toPlay;
		deallocMoveStruc();

		int toReturn;

		if ( q == 0 )		//game ends!!!!
		{
			int i = diskDifference( toPlay );

			if ( i > 0 )
			{
				toReturn = scoreWin + i;
			}
			else if( i < 0 )
			{
				toReturn = scoreLoss - i;
			}
			else 
			{
				toReturn = scoreDraw;
			}
		}
		else		// its just a pass from our side, so evaluate & return the opponent's move
		{
			toPlay = -toPlay;
			toReturn = -evaluate( -beta, -alpha, depthThisTime - 1 );
			toPlay = -toPlay;
		}

		deallocMoveStruc();
		curScore = toReturn;
		return 64;    //***** 64 indicates illegal move - here it means no move available
	}

	//order the moves for best pruning
	int moveOrder[40] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
		25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
	orderMoves( numMoves, moveStruc, moveOrder );

	//make and evaluate the tree
	int v = alpha;
	int e;
	int bestMove;
	for (int i = 0; i < numMoves; i++)
	{
		playMove( moveStruc, moveOrder[i] );

		toPlay = -toPlay;
		e = -evaluate( -beta, -v, depthThisTime - 1 );
		toPlay = -toPlay;

		undoMove( moveStruc, moveOrder[i] );

		if (e > v)
		{ 
			v = e;
			bestMove = moveOrder[i]; 
		}
	}
	bestMove = moveStruc[ 20 * bestMove ];

	deallocMoveStruc();

	curScore = v;
	printf( "AI move: %d\n", bestMove );
	return bestMove;
}

int ReversiAI::getCurScore()
{
	return curScore;
}

long int ReversiAI::getLeavesEvaluated() 
{ 
	return count; 
}

int ReversiAI::setUpLegalMoves( char* _moveStruc )
//returns no of legal moves and sets up the legal moves in the move structure move_struc
{
	int move_no = 0;
	int who = toPlay;
	int other = -toPlay;

	for (int z = 0; z <= 59; z++ )
	{
		int m = order[z];

		_moveStruc[move_no * 20] = m;	//put m on it for trial
		int move_effect = 1;  //it will be overwritten next loop if it
									// does not present itself as legal move

		//begin testing
		int i,j;
		int r = m/8, c = m%8;

		if (board[m]) continue;

		if (c<=5)
			if (board[m+1]==other)
				for ( i=m+2; i%8!=0; i++)
					{
					if (board[i]==other) continue;
					if (board[i]==who)
						for (j=m+1; j!=i; j++)
							{
							_moveStruc[move_no*20 + move_effect] = j;
							move_effect++;
							}
					break;
					}
		if (c>=2)
			if (board[m-1]==other)
				for ( i=m-2; i%8!=7 && i!=-1; i--)
					{
					if (board[i]==other) continue;
					if (board[i]==who)
						for ( j=m-1; j!=i; j--)
							{
							_moveStruc[move_no*20 + move_effect] = j;
							move_effect++;
							}
					break;
					}
		if (r>=2)
			if (board[m-8]==other)
				for ( i=m-16; i>=0 ; i-=8)
					{
					if (board[i]==other) continue;
					if (board[i]==who)
						for (j=m-8; j!=i; j-=8)
							{
							_moveStruc[move_no*20 + move_effect] = j;
							move_effect++;
							}
					break;
					}
		if (r<=5)
			if (board[m+8]==other)
				for ( i=m+16; i<=63 ; i+=8)
					{
					if (board[i]==other) continue;
					if (board[i]==who)
						for (j=m+8; j!=i; j+=8)
							{
							_moveStruc[move_no*20 + move_effect] = j;
							move_effect++;
							}
					break;
					}
		if (r<=5 && c<=5)
			if (board[m+9]==other)
				for ( i=m+18; (i<=63) && ((i%8)!=0) ; i+=9)
					{
					if (board[i]==other) continue;
					if (board[i]==who)
						for (j=m+9; j!=i; j+=9)
							{
							_moveStruc[move_no*20 + move_effect] = j;
							move_effect++;
							}
					break;
					}
		if (r<=5 && c>=2)
			if (board[m+7]==other)
				for ( i=m+14; (i<=63) && ((i%8)!=7) ; i+=7)
					{
					if (board[i]==other) continue;
					if (board[i]==who)
						for ( j=m+7; j!=i; j+=7)
							{
							_moveStruc[move_no*20 + move_effect] = j;
							move_effect++;
							}
					break;
					}
		if (r>=2 && c<=5)
			if (board[m-7]==other)
				for ( i=m-14; i>=0 && i%8 != 0 ; i-=7)
					{
					if (board[i]==other) continue;
					if (board[i]==who)
						for (j=m-7; j!=i; j-=7)
							{
							_moveStruc[move_no*20 + move_effect] = j;
							move_effect++;
							}
					break;
					}
		if (r>=2 && c>=2)
			if (board[m-9]==other)
				for ( i=m-18; (i>=0) && ((i%8)!=7) ; i-=9)
					{
					if (board[i]==other) continue;
					if (board[i]==who)
						for (j=m-9; j!=i; j-=9)
							{
							_moveStruc[move_no*20 + move_effect] = j;
							move_effect++;
							}
					break;
					}

		if (move_effect>1)
			{
			if (move_effect<20) _moveStruc[move_no*20 + move_effect] = (char)100;	//indicate termination
			move_no++;	//set pointer ready for next legal move
			}

		}

	_moveStruc[ move_no * 20 ] = (char)100;

	//ASSERT(move_no<34);

	return move_no;

}

char* ReversiAI::allocMoveStruc()
{
	usedMoveStruc++;
	return startMoveStruc + ( ( usedMoveStruc - 1 ) * maxDisksTurned * maxLegalMoves );
}

void ReversiAI::deallocMoveStruc()
{
	usedMoveStruc--;
}

int ReversiAI::diskDifference( int _toPlay )
{
	int i;
	int me = 0;
	int you = 0;

	for ( i = 0; i <= 63; i++ )
	{
		if ( board[i] == _toPlay) 
		{
			me++;
		}
		else if( board[i] == -_toPlay ) 
		{
				you++;
		}
	}

	return me - you;
}

void ReversiAI::orderMoves( int _numMoves, char* _moveStruc, int* _moveOrder )
//searches to a shallow depth of move_order_depth(generally 2) and sorts the moves by the
//evaluations thus returned. Because of good move sorting, alpha-beta is even faster
// one complication is that this function should not go on recursing, this will slow things.
// so, the shallow search initiated by this function is marked by the boolean variable
// move_order_search, which will not let this function be called until true search is resumed
{
	//now its a true search, not the search initiated by order_moves() itself
	moveOrderSearch = true;

	int i,j;

	for ( i = 0; i < _numMoves; i++ )
	{
		playMove( _moveStruc, i );

		toPlay = -toPlay;
		evalTable[i] = -evaluate( startAlpha, startBeta, moveOrderDepth - 1 );
		toPlay = -toPlay;

		undoMove( _moveStruc, i );
	}

	//sort the evaluations
	for ( i = 0; i < _numMoves; i++ )
	{
		int max = -1000000L;
		int maxMove;
		for( j = 0; j < _numMoves; j++ )
		{
			if ( evalTable[j] > max ) 
			{ 
				max = evalTable[j]; 
				maxMove = j; 
			}
		}
		_moveOrder[i] = maxMove;
		evalTable[ maxMove ] = -1000000L;
	}

	moveOrderSearch = false;
}

void ReversiAI::playMove( char* _moveStruc, int _movenum )
{
	int t;
	for ( int i = 0; i <= 19; i++ )
	{
		t = _moveStruc[ _movenum * 20 + i ];
		if ( t == 100 )
		{
			break;
		}
		board[t] = toPlay;
	}
	//assert(t==100);
	totalDisks++;
}

void ReversiAI::undoMove( char* _moveStruc, int _movenum )
{
	board[ _moveStruc[ _movenum * 20 ] ] = 0;	//make empty the square where the piece was placed

	int t;
	for (int i = 1; i <= 19; i++ )
	{
		t = _moveStruc[ _movenum * 20 + i ];
		if ( t == 100 )
		{ 
			break;
		}
		board[t] = -toPlay;
	}
	//assert(t==100);
	totalDisks--;
}


int ReversiAI::opp( int _who ) 
{ 
	return -_who; 
}

int ReversiAI::strat1( int _who )
{                              //edge discs give more power
	static int starts[4] = { 0, 7, 56, 63 };
	static int incs[8] = { 1, 8, -1, 8, 1, -8, -1, -8};
	int i,j;
	int corner;
	int toReturn=0;

	for( corner = 0; corner <= 3; corner++ )
	{
		int cell = starts[ corner ];

		if ( board[cell] != _who ) 
		{
			continue;
		}

		toReturn += 100;

		for( i = 1; i <= 6; i++ )
		{
			j = cell + i * incs[ corner * 2 ];
			if ( board[j] != _who ) 
			{
				break;
			}
			toReturn += ( i * 8 );
		}
		for( i = 1; i <= 6; i++ )
		{
			j = cell + i * incs[ corner * 2 + 1 ];
			if ( board[j] != _who )
			{
				break;
			}
			toReturn += ( i * 8 );
		}
	}

	return toReturn;

}

int ReversiAI::strat3( int _who )
{
	return noOfLegalMoves( _who );
}

int ReversiAI::strat4( int _who )
{
	int toReturn = 0;
	int you = -_who;

	for( int i = 0; i <= 63; i++ )
	{
		if ( board[i] == 0 )
		{
			int r = i / 8;
			int c = i % 8;
			if ( r <= 5 )
			{
				toReturn += ( ( board[ i + 8 ] == you )? 1 : 0);
			}
			if (  r>= 2 )
			{
				toReturn += ( ( board[ i - 8 ] == you )? 1 : 0);
			}
			if ( c <= 5 ) 
			{
				toReturn += ( ( board[ i + 1 ] == you )? 1 : 0);
			}
			if ( c >= 2 )
			{
				toReturn += ( ( board[ i - 1 ] == you )? 1 : 0);
			}
			if ( r <= 5 && c <= 5 )
			{
				toReturn += ( ( board[ i + 9 ] == you )? 1 : 0);
			}
			if ( r <= 5 && c >= 2) 
			{
				toReturn += ( ( board[ i + 7 ]== you )? 1 : 0);
			}
			if ( r >= 2 && c <= 5 ) 
			{
				toReturn += ( ( board[ i - 7 ] == you )? 1 : 0);
			}
			if ( r >= 2 && c >= 2 )
			{
				toReturn += ( ( board[ i - 9 ] == you )? 1 : 0);
			}
		}
	}

	return toReturn;
}

int ReversiAI::noOfLegalMoves( int _who )
{
	int toReturn = 0;
	for( int i = 0; i <= 63; i++ )
	{
		if ( isLegal( _who, i ) ) 
		{
			toReturn++;
		}
	}

	return toReturn;
}

bool ReversiAI::isLegal( int _who, int _m )
{
	if ( board[_m] )
	{
		return false;
	}

	int r = _m / 8;
	int c = _m % 8;
	int you = opp( _who );

	if ( c <= 5 )
	{
		if ( board[ _m + 1 ] == you )
		{
			for ( int i = _m + 2; i % 8 != 0; i++ )
			{
				if ( board[i] == _who ) 
				{
					return true;
				}
				else if ( board[i] == 0 ) 
				{
					break;
				}
			}
		}
	}
	if ( c >= 2 )
	{
		if ( board[ _m - 1 ] == you )
		{
			for ( int i = _m - 2; i % 8 != 7 && i != -1; i-- )
			{
				if ( board[i] == _who ) 
				{
					return true;
				}
				else if ( board[i] == 0 ) 
				{
					break;
				}
			}
		}
	}
	if ( r >= 2 )
	{
		if ( board[ _m - 8 ] == you )
		{
			for ( int i = _m - 16; i >= 0 ; i -= 8 )
			{
				if ( board[i] == _who ) 
				{
					return true;
				}
				else if ( board[i] == 0 ) 
				{
					break;
				}
			}
		}
	}
	if ( r <= 5 )
	{
		if ( board[ _m + 8 ] == you )
		{
			for ( int i = _m + 16; i <= 63 ; i += 8 )
			{
				if ( board[i] == _who )
				{
					return true;
				}
				else if( board[i] == 0 ) 
				{
					break;
				}
			}
		}
	}
	if ( r <= 5 && c <= 5 )
	{
		if ( board[ _m + 9 ] == you )
		{
			for ( int i = _m + 18; ( i <= 63 ) && ( ( i % 8 ) != 0 ) ; i += 9 )
			{
				if ( board[i] == _who ) 
				{
					return true;
				}
				else if ( board[i] == 0 )
				{
					break;
				}
			}
		}
	}
	if ( r <= 5 && c >= 2 )
	{
		if ( board[ _m + 7 ] == you )
		{
			for ( int i = _m + 14; ( i <= 63 ) && ( ( i % 8 ) != 7 ) ; i += 7 )
			{
				if ( board[i] == _who ) 
				{
					return true;
				}
				else if ( board[i] == 0 ) 
				{
					break;
				}
			}
		}
	}
	if ( r >= 2 && c <= 5 )
	{
		if ( board[ _m - 7 ] == you )
		{
			for ( int i = _m - 14; i >= 0 && i % 8 != 0 ; i -= 7 )
			{
				if ( board[i] == _who ) 
				{
					return true;
				}
				else if ( board[i] == 0 )
				{
					break;
				}
			}
		}
	}
	if ( r >= 2 && c >= 2 )
	{
		if ( board[ _m - 9 ] == you )
		{
			for ( int i = _m - 18; ( i >= 0 ) && ( ( i % 8 ) != 7 ) ; i -= 9 )
			{
				if ( board[i] == _who ) 
				{
					return true;
				}
				else if ( board[i] == 0 )
				{
					break;
				}
			}
		}
	}

	return false;
}
	
int ReversiAI::evaluate( int _alpha, int _beta, int _height )
{
	if ( _height == 0 )	//we have reached a leaf
	{
		return statEval();
	}

	char* moveStruc;
	moveStruc = allocMoveStruc();

	int numMoves = setUpLegalMoves( moveStruc );

	if ( numMoves == 0 )  //no moves to play for us!
	{
		char* tempMoveStruc;
		int q;

		tempMoveStruc = allocMoveStruc();
		toPlay = -toPlay;
		q = setUpLegalMoves( tempMoveStruc );
		toPlay = -toPlay;
		deallocMoveStruc();

		int toReturn;

		if ( q == 0 )		//game ends!!!!
		{
			int i = diskDifference( toPlay );

			if (i>0)
			{
				toReturn = scoreWin + i;
			}
			else if( i < 0 )
			{
				toReturn = scoreLoss + i;
			}
			else 
			{
				toReturn = scoreDraw;
			}
		}
		else		// its just a pass from our side, so evaluate & return the opponent's move
		{
			toPlay = -toPlay;
			toReturn = -evaluate( -_beta, -_alpha, _height - 1 );
			toPlay = -toPlay;
		}

		deallocMoveStruc();
		return toReturn;
	}

	//order the moves for best pruning
	int moveOrder[40] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
		25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
	if ( ( moveOrderSearch == false ) && ( _height >= 4 ) )
	{
		orderMoves( numMoves, moveStruc, moveOrder );
	}

	//make and evaluate the tree
	int v = _alpha;
	int e;
	for ( int i = 0; i < numMoves; i++ )
	{
		playMove( moveStruc, moveOrder[i] );

		toPlay = -toPlay;
		e = -evaluate( -_beta, -v, _height - 1 );
		toPlay = -toPlay;

		undoMove( moveStruc, moveOrder[i] );

		if (e > v) 
		{
			v = e;
		}
		if (v >= _beta)
		{
			break;	//alpha-beta cut-off
		}
	}

	deallocMoveStruc();

	return v;
}
int ReversiAI::statEval()
{
	count++;

	int sum;
	int sum1 = 0;
	int sum2 = 0;
	int sum4 = 0;
	int sum5 = 0;
	float a,b,d,e;

	if      (totalDisks < 20) { a = 9; b = 2; d = 9; e = 6; }
	else if (totalDisks < 30) { a = 9; b = 2; d = 9; e = 6; }
	else if (totalDisks < 40) { a = 9; b = 2; d = 9; e = 6; }
	else if (totalDisks < 48) { a = 9; b=1.7f; d = 8; e = 5; }
	else if (totalDisks < 53) { a = 9; b=1.2f; d = 7; e = 4; }
	else 							  { a = 9; b = 0; d = 0; e = 0; }

	int t1 = strat3( toPlay );
	int t2 = strat3( -toPlay );
	int t = diskDifference( toPlay );

	if ( t1 == 0 && t2 == 0 )					//game has ended
	{
		if ( t > 0 )
		{
			return scoreWin + t;
		}
		else if( t < 0 )
		{
			return scoreLoss - t;
		}
		else 
		{
			return scoreDraw;
		}
	}

	sum1 = t;
	sum2 = strat1( toPlay ) - strat1( -toPlay );
	sum4 = t1 - t2;
	sum5 = strat4( toPlay ) - strat4( -toPlay );

	sum = int( a * sum1 + b * sum2 + d * sum4 + e * sum5 );
	return sum;
}

int ReversiAI::getLegalMovesCount( int _playerID )
{
	int mark = 0;
	if( _playerID == meId )
		mark = 1;
	else if( _playerID == youId )
		mark = -1;
	return noOfLegalMoves( mark );
}
