#	pragma once

typedef struct 
{
	int x;
	int y;
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

	int getState( int _x, int _y );
	int getTurn();
	void setInitialPosition(int** board);
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
	void delMoveList( TMoveList* _list); // Удалить список из памяти
	// строит дерево возможных ходов для шашки
	void createTree( TMovesTree** _head, TMovesTree* _parent, int _x, int _y, bool _isSimple = true );
	// Процедура инициализирует узел дерева заданными значениями}
	void initialize( TMovesTree** _head, TMovesTree* _parent, int _x, int _y );
	// Функция проверяет не были ли мы здесь раньше
	bool isFirstTime( TMovesTree* _node, int _x, int _y );
	// на основание дерева ходов добавляет элементы в список ходов
	void treeToList( TMovesTree* _tree, TMovesTree* _node, TMoveList** _list );
	// освобождает память, выделенную дереву
	void deleteTree( TMovesTree* _head );

	int goalFunc();
	int goalFunc2();
	int getBestMove( TMove& _bestMove, int _level );
	int getBestMove2( TMove& _bestMove, int _level );
};
