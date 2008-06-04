#	pragma once

typedef struct _TPlace
{
	int x;
	int y;
	_TPlace()
	{
	}
	_TPlace( int _x, int _y )
		:x( _x )
		,y( _y )
	{
	}
} TPlace;

typedef struct  
{
	TPlace moveFrom;
	TPlace moveTo;
} TMove;

typedef struct _TMoveList
{
	TMove move;
	_TMoveList* next;
	_TMoveList* prev;
} TMoveList;

typedef struct _TMovesTree
{
	TPlace disposition;
	_TMovesTree* root;
	_TMovesTree* simpleStep[4];
	_TMovesTree* jumpStep[4];
	_TMovesTree( _TMovesTree* _parent, int _x, int _y )
		: disposition( _x, _y )
		, root( _parent )
	{
		for( int i = 0; i < 4; i++ )
		{
			simpleStep[i] = 0;
			jumpStep[i] = 0;
		}
	}
} TMovesTree;

enum TDirection
{
	Right = 0,
	Up,
	Left,
	Down
};

enum TState
{
	Space = 0,
	White,
	Black
};

class CornersAI
{
public:
	CornersAI();
	CornersAI( const CornersAI& _other );
	~CornersAI();

	void setInitialPosition(int* board);
	void changeTurn();
	void restartGame();
	void setPiece( int _x, int _y, int _player );
	void setPiece( const TPlace& _place, int _player );
	void removePiece( int _x, int _y );
	void remvoePiece( const TPlace& _place );
	void makeMove( int _x1, int _y1, int _x2, int _y2 );
	//void makeMove( const TPlace& _start, const TPlace& _end );
	//void makeMove( const TMove& _move );
	bool isMovePossible( int _x1, int _y1, int _x2, int _y2 );
	bool isMovePossible( const TPlace& _start, const TPlace& _end );
	bool isMovePossible( const TMove& _move );
	int autoMove( int _level );
	int getWinner();
	
private:
	int m_board[8][8];
	int m_initialBoard[8][8];
	int m_turn;
	bool m_breakFlag;
	//typedef list<TMove> TListMove;

	TMoveList* getMoveList();
	void delMoveList( TMoveList* _list); // ������� ������ �� ������
	// ������ ������ ��������� ����� ��� �����
	void createTree( TMovesTree** _head, TMovesTree* _parent, int _x, int _y, bool _isSimple = true );
	// ��������� �������������� ���� ������ ��������� ����������}
	void initialize( TMovesTree** _head, TMovesTree* _parent, int _x, int _y );
	// ������� ��������� �� ���� �� �� ����� ������
	bool isFirstTime( TMovesTree* _node, int _x, int _y );
	// �� ��������� ������ ����� ��������� �������� � ������ �����
	void treeToList( TMovesTree* _tree, TMovesTree* _node, TMoveList** _list );
	// ����������� ������, ���������� ������
	void deleteTree( TMovesTree* _head );

	int goalFunc();
	int goalFunc2();
	int getBestMove( TMove& _bestMove, int _level );
	int getBestMove2( TMove& _bestMove, int _level );
};
