#	pragma once

#define BADMOVE	-1

const int NONE = 0;
const int BLACK = 1;
const int WHITE = 2;
const int POSSIBLE = 3;

enum ePlayerType { HUMAN, COMPUTER };

class ReversiAI  
{
public:
	ReversiAI();
	~ReversiAI();

	void setId( int _me, int _you, int _empty );	//set nID's used by the user interface
	void setDepth( int _d );						//set depth to be searched
	void setEndgameEmpty( int _d );					//set the start of endgame with d squares empty

	void markCell( int _playerID, int _x, int _y );
	int autoMove();
	int getCurScore();
	long int getLeavesEvaluated();
	int getLegalMovesCount( int _playerID );

private:
	int board[64];
	int meId;
	int youId;
	int emptyId;

	int toPlay; // { +1, -1 } the player who has the turn
	int moveOrderDepth;
	int depth;
	int egEmpty;

	long int count; //no of leaves examined
	bool memErr;

	int usedMoveStruc;
	char* startMoveStruc;

	int maxDepth;
	int maxDisksTurned;
	int maxLegalMoves;

	int totalDisks;

	int order[60];
	int scoreWin;
	int scoreLoss;
	int scoreDraw;
	int startAlpha, startBeta;
	int curScore;

	bool moveOrderSearch;
	int evalTable[40];

	int setUpLegalMoves( char* _moveStruc );	//sets up legal moves in c and returns no of legal moves
	char* allocMoveStruc();
	void deallocMoveStruc();
	inline int diskDifference( int _toPlay );
	inline void orderMoves( int _numMoves, char* _moveStruc, int* _moveOrder);
	inline void playMove( char* _moveStruc, int _movenum );
	inline void undoMove( char* _moveStruc, int _movenum );

	int opp( int _who );
	int strat1( int _who );  //corner discs COMBINED with CONSECUTIVE
	int strat3( int _who ); //mobility
	int strat4( int _who ); //potential mobility
	int noOfLegalMoves( int _who );
	bool isLegal( int _who, int _m);

	int evaluate( int _alpha, int _beta, int _height );
	int statEval();

};
