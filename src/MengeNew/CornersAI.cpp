#	include "CornersAI.h"

#	include <memory>

#define MIN(a, b)	( (a)>(b)?(b):(a) )
#define MAX(a, b)	( (a)>(b)?(a):(b) )
#define RANDOM(min, max)	( (((double) rand() / (double) (RAND_MAX)) * (max) + (min)))

CornersAI::CornersAI()
: m_turn( 1 )
{
	int initial[8][8] = 
	{
		{ 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 1, 1, 1, 0, 0, 0, 0, 0 },
		{ 1, 1, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 2 },
		{ 0, 0, 0, 0, 0, 0, 2, 2 },
		{ 0, 0, 0, 0, 0, 2, 2, 2 },
		{ 0, 0, 0, 0, 2, 2, 2, 2 }
	};
	setInitialPosition( &initial[0][0] );
}

CornersAI::CornersAI( const CornersAI& _other )
: m_turn( _other.m_turn )
{
	memcpy( &m_board[0][0], &_other.m_board[0][0], 64 * sizeof( int ) );
	memcpy( &m_initialBoard[0][0], &_other.m_initialBoard[0][0], 64 * sizeof( int ) );
}

CornersAI::~CornersAI()
{

}

void CornersAI::setInitialPosition( int* board )
{
	memcpy( &m_initialBoard[0][0], board, 64 * sizeof( int ) );
}

void CornersAI::changeTurn()
{
	if( m_turn == 1 )
	{
		m_turn = 2;
	}
	else
	{
		m_turn = 1;
	}
	//printf( "changeTurn %d\n", m_turn );
}

void CornersAI::restartGame()
{
	memcpy( &m_board[0][0], &m_initialBoard[0][0], 64 * sizeof( int ) );
	m_turn = 1;
}

TMoveList* CornersAI::getMoveList()
{
	TMoveList* list = NULL;	// ��������� �� �������������� ������
	TMovesTree* tree = NULL;	// ��������� �� "������" ������
	int x;
	int y;
	list = NULL;
	// ��� ������ ����� �������� �����
	for( x = 0; x < 8; x++ )
	{
		for( y = 0; y < 8; y++ )
		{
			if( m_board[x][y] == m_turn )
			{
				// ��������� ������
				createTree( &tree, NULL, x, y );

				// �������� � ������
				treeToList( tree, tree, &list );

				// ���������� ������
				deleteTree( tree );

			}
		}
	}
	return list;
}

void CornersAI::createTree( TMovesTree** _head, TMovesTree* _parent, int _x, int _y, bool _isSimple /* = true  */ )
{
	// �������������
	(*_head) = new TMovesTree( _parent, _x, _y );
	// �������� ��������� ����������� ����
	if (_isSimple)
	{
		// ������� ���� �������������� ������ ��� ������� ����
		// �������� �� ����������, � ������ ��������� �������� ����
		if ( _x != 7 && m_board[_x+1][_y] == TState::Space )
		{
			(*_head)->simpleStep[TDirection::Right] = new TMovesTree( (*_head), _x+1, _y );
			//initialize( &(*_head)->simpleStep[TDirection::Right], (*_head), _x+1, _y );
		}
		if ( _x != 0 && m_board[_x-1][_y] == TState::Space )
		{
			(*_head)->simpleStep[TDirection::Left] = new TMovesTree( (*_head), _x-1, _y );
			//initialize( &(*_head)->simpleStep[TDirection::Left], (*_head), _x-1, _y );
		}
		if ( _y != 7 && m_board[_x][_y+1] == TState::Space )
		{
			(*_head)->simpleStep[TDirection::Up] = new TMovesTree( (*_head), _x, _y+1 );
			//initialize( &(*_head)->simpleStep[TDirection::Up], (*_head), _x, _y+1 );
		}
		if ( _y != 0 && m_board[_x][_y-1] == TState::Space )
		{
			(*_head)->simpleStep[TDirection::Down] = new TMovesTree( (*_head), _x, _y-1 );
			//initialize( &(*_head)->simpleStep[TDirection::Down], (*_head), _x, _y-1 );
		}
	}
	// �������������� ��� ������ ���� - ������
	// ���������� ���������� CreateTree
	// ������
	if ( _x != 7 
		&& _x+1 != 7 
		&& m_board[_x+1][_y] != TState::Space 
		&& m_board[_x+2][_y] == TState::Space 
		&& isFirstTime( (*_head), _x+2, _y ) )
	{
		createTree( &(*_head)->jumpStep[TDirection::Right], (*_head), _x+2, _y, false );
	}
	// �����
	if ( _x != 0 
		&& _x-1 != 0 
		&& m_board[_x-1][_y] != TState::Space 
		&& m_board[_x-2][_y] == TState::Space 
		&& isFirstTime( (*_head), _x-2, _y ) )
	{
		createTree( &(*_head)->jumpStep[TDirection::Left], (*_head), _x-2, _y, false);
	}
	// �����
	if ( _y != 7 
		&& _y+1 != 7 
		&& m_board[_x][_y+1] != TState::Space 
		&& m_board[_x][_y+2] == TState::Space 
		&& isFirstTime( (*_head), _x, _y+2 ) )
	{
		createTree( &(*_head)->jumpStep[TDirection::Up], (*_head), _x, _y+2, false );
	}
	// ����
	if ( _y != 0 
		&& _y-1 != 0 
		&& m_board[_x][_y-1] != TState::Space 
		&& m_board[_x][_y-2] == TState::Space 
		&& isFirstTime( (*_head), _x, _y-2 ) )
	{
		createTree( &(*_head)->jumpStep[TDirection::Down], (*_head), _x, _y-2, false );
	}
}

bool CornersAI::isFirstTime( TMovesTree* _node, int _x, int _y )
{
	TMovesTree* curNode = _node->root;
	while ( curNode ) 
	{
		if( curNode->disposition.x == _x && curNode->disposition.y == _y )
		{
			return false;
		}
		curNode = curNode->root;
	}
	return true;
}

void CornersAI::treeToList( TMovesTree* _tree, TMovesTree* _node, TMoveList** _list )
{
	TMoveList* temp;
	bool firstTime;

	// ��������� �� �������� �� ������, � ���� ��������, �� ���������� ����������
	if ( _tree != _node )
	{
		// ��������� ���� �� ������� ��� � ������ � ���� ���� - ��������
		firstTime = true;
		temp = (*_list);
		while ( temp != NULL && temp->move.moveFrom.x == _tree->disposition.x && temp->move.moveFrom.y == _tree->disposition.y )
		{
			if ( _node->disposition.x == temp->move.moveTo.x && _node->disposition.y == temp->move.moveTo.y )
			{
				firstTime = false;
			}
			temp = temp->next;
		}
		if ( firstTime )
		{
			temp = new TMoveList;									//������� ����� �������
			temp->move.moveFrom.x = _tree->disposition.x;			//�������������������
			temp->move.moveFrom.y = _tree->disposition.y;
			temp->move.moveTo.x = _node->disposition.x;
			temp->move.moveTo.y = _node->disposition.y;
			if ( (*_list) != NULL) 
			{
				(*_list)->prev = temp;                           // � ������ � ������ ������
			}
			temp->next = (*_list);
			temp->prev = NULL;
			(*_list) = temp;
		}
	}
	// ���������� ������� ���� ��� ���� "�������" ������ �� ������ ����}
	for ( int i = 0; i < 4; i++ )
	{
		if ( _node->simpleStep[i] != NULL )
		{
			treeToList( _tree, _node->simpleStep[i], _list);
		}
		if ( _node->jumpStep[i] != NULL )
		{
			treeToList( _tree, _node->jumpStep[i], _list);
		}
	}
}

void CornersAI::deleteTree( TMovesTree* _head )
{
	// ������� ������ ����������� ����
	for ( int i = 0; i < 4; i++ )
	{
		if ( _head->simpleStep[i] != NULL )
		{
			deleteTree( _head->simpleStep[i] );
		}
		if ( _head->jumpStep[i] != NULL )
		{
			deleteTree( _head->jumpStep[i] );
		}
	}
	//� ������ ������ ��� ����
	delete _head;
}

void CornersAI::delMoveList( TMoveList* _list )
{
	TMoveList* temp;
	while ( _list )
	{
		temp = _list->next;
		delete _list;
		_list = temp;
	}
}

int CornersAI::goalFunc()
{
	int	i;
	int j;
	int WhiteR = 0;
	int BlackR = 0;
	// ������� ����������
	for ( i = 0; i < 8; i++ )
	{
		for ( j = 0; j < 8; j++ )
		{
			if ( m_board[i][j] == TState::White )
			{
				// ����� ���������� �� ���� ������
				// WhiteR:=WhiteR+16-ord(i)-ord(j);
				WhiteR = WhiteR + ( 16 - MIN(i, 7) - MIN(j, 7) );
			}
			if ( m_board[i][j] == TState::Black )
			{
				// ����� ���������� �� ���� �����
				// BlackR:=BlackR+ord(i)+ord(j);
				BlackR = BlackR + ( MAX(i, 2) + MAX(j, 2) );
			}
		}
	}
	if ( m_turn == TState::Black )
	{
		return ( WhiteR - BlackR )*10 + RANDOM(0, 10);
	}
	//else
	return ( BlackR - WhiteR )*10 + RANDOM(0, 10);
}

int CornersAI::goalFunc2()
{
	int i;
	int j;
	int WhiteR = 0;
	int BlackR = 0;
	int WhiteCount = 0;
	int BlackCount = 0;
	// ������� ����������
	for ( i = 0; i < 8; i++ )
	{
		for ( j = 0; j < 8; j++ )
		{
			if ( m_board[i][j] == TState::White )
			{
				// ���������� ����� ���������� �� ���� ������
				if ( i < 4 || j < 2 )
				{
					WhiteR = WhiteR + ( 16 - i - j )*5;
				}
				else
				{
					WhiteR = WhiteR + 16 - i - j;
				}
				// ���������� ��������� ������ �����
				if ( m_initialBoard[i][j] == TState::Black )
				{
					WhiteCount++;
				}
			}
			else if ( m_board[i][j] == TState::Black )
			{
				// ���������� ����� ���������� �� ���� �����
				if ( i > 5 || j > 5)
				{
					BlackR = BlackR + ( i + j )*5;
				}
				else
				{
					BlackR = BlackR + i + j;
				}
				// ���������� ��������� ������ �����}
				if ( m_initialBoard[i][j] == TState::White )
				{
					BlackCount++;
				}
			}
		}
	}

	if ( m_turn == TState::Black )
	{
		return +BlackCount*50 - BlackR*10 + RANDOM(0, 10);
	}
	// else
	return WhiteCount*50 - WhiteR*10 + RANDOM(0, 10);
}

void CornersAI::setPiece( int _x, int _y, int _player )
{
	m_board[_x][_y] = _player;
}

void CornersAI::setPiece( const TPlace& _place, int _player )
{
	m_board[_place.x][_place.y] = _player;
}

void CornersAI::removePiece( int _x, int _y )
{
	m_board[_x][_y] = TState::Space;
}

void CornersAI::remvoePiece( const TPlace& _place )
{
	m_board[_place.x][_place.y] = TState::Space;
}

void CornersAI::makeMove( int _x1, int _y1, int _x2, int _y2 )
{
	m_board[_x2][_y2] = m_board[_x1][_y1];
	m_board[_x1][_y1] = TState::Space;
}

/*void CornersAI::makeMove( const TPlace& _start, const TPlace& _end )
{
	makeMove( _start.x, _start.y, _end.x, _end.y );
}

void CornersAI::makeMove( const TMove& _move )
{
	makeMove( _move.moveFrom, _move.moveTo );
}*/

bool CornersAI::isMovePossible( int _x1, int _y1, int _x2, int _y2 )
{
	TMoveList* list = getMoveList();
	TMoveList* curMove = list;
	bool possible = false;
	while ( curMove )
	{
		if (curMove->move.moveFrom.x == _x1 && curMove->move.moveFrom.y == _y1 && curMove->move.moveTo.x == _x2 && curMove->move.moveTo.y == _y2 )
		{
			possible = true;
		}
		curMove = curMove->next;
	}
	delMoveList( list );
	return possible;
}

bool CornersAI::isMovePossible( const TPlace& _start, const TPlace& _end )
{
	return isMovePossible( _start.x, _start.y, _end.x, _end.y );
}

bool CornersAI::isMovePossible( const TMove& _move )
{
	return isMovePossible( _move.moveFrom, _move.moveTo );
}

int CornersAI::autoMove( int _level )
{
	int i;
	int j;
	int Count = 0;
	int WhiteR = 0;
	int BlackR = 0;
	int WhiteSp = 0;
	int BlackSp = 0;

	TMove Result;

	//printf( "AI move %d\n", m_turn );
	// ��������� ����� ���������� �� �������� ����
	for ( i = 0; i < 8; i++ )
	{
		for ( j = 0; j < 8; j++ )
		{
			if ( m_board[i][j] == TState::White )
			{
				WhiteR = WhiteR + 16 - i - j;
				Count++;
			}
			else if ( m_board[i][j] == TState::Black )
			{
				BlackR = BlackR + i + j;
			}
			if( m_initialBoard[i][j] == TState::White && m_board[i][j] != TState::Black )
			{
				BlackSp++;
			}
			else if( m_initialBoard[i][j] == TState::Black && m_board[i][j] != TState::White )
			{
				WhiteSp++;
			}
		}
	}

	// ������� �������� ���������� ���������� ����
	if ( ( m_turn == TState::White && WhiteR > Count*5 ) || ( m_turn == TState::Black && BlackR > Count*5) )
	{
		// ����� ���� �� �������� ������ �������� ������
		getBestMove( Result, _level );
	}
	else
	{
		// ����� ���� �� �������� ������ �������� ������
		if ( ( m_turn == TState::White && WhiteSp <= 3 ) || ( m_turn == TState::Black && BlackSp <= 3 ) )
		{
			getBestMove2( Result, MAX( _level, 3) );
		}
		else
		{
			getBestMove2( Result, _level );
		}
	}

	// x1-y1-x2-y2
	for( int i = 0; i < 8; i++ )
	{
		for(int j = 0; j < 8; j++)
		{
			printf("%d ", m_board[j][i] );
		}
		printf("\n");
	}

	printf( "AI moves\nfrom: %d %d - to: %d %d\n", Result.moveFrom.x, Result.moveFrom.y, Result.moveTo.x, Result.moveTo.y );
	return Result.moveFrom.x*1000 + Result.moveFrom.y*100 + Result.moveTo.x * 10 + Result.moveTo.y;
}

int CornersAI::getBestMove( TMove& _bestMove, int _level )
{
	TMoveList* list;
	TMoveList* curMove;
	CornersAI* P;
	int BestScore;
	int Score;
	TMove Move;
	int MovesCount = 0;
	int MoveNum = 0;

	if ( _level )
	{
		// ������� ������ ��������� �����
		list = getMoveList();
		// ���� ����������� - ��������� �����
		// ������� ������ Position
		P = new CornersAI( *this );
		// ������� ����� ������� �������

		// ������������� �����
		curMove = list;
		BestScore = -10000;
		// ��� ������� ���������� ����:
		while ( curMove )
		{
			//���������� ���
			P->makeMove( curMove->move.moveFrom.x, curMove->move.moveFrom.y, curMove->move.moveTo.x, curMove->move.moveTo.y );
			// �������� ��� ���������
			P->changeTurn();
			// ������� GetBestMove ��� ���������� �������
			Score = -P->getBestMove( Move, _level - 1);
			// O����������� ������ �� ��� ���
			if ( Score > BestScore )
			{
			  BestScore = Score;
			  _bestMove = curMove->move;
			}
			// ������� ��� �����
			//P->makeMove( curMove->move.moveTo, curMove->move.moveFrom );
			P->makeMove( curMove->move.moveTo.x, curMove->move.moveTo.y, curMove->move.moveFrom.x, curMove->move.moveFrom.y );
			P->changeTurn();
			// ��������� ���
			curMove = curMove->next;
		}
		// ������� ������ ��������� �����}
		delMoveList( list );
		// ���������� ������}
		delete P;
		// ������� ������ ������� ����
		return BestScore;
	}
	else // ������ ������ ��������� �� _level
	{
	  // ������� ������� �������
	  // ������� �������� ������� �������
	  return goalFunc();
	}
}

int CornersAI::getBestMove2( TMove& _bestMove, int _level )
{
    TMoveList* list;
    TMoveList* curMove;
    CornersAI* P;
    int BestScore;
	int Score;
    TMove Move;
    int MovesCount = 0;
	int MoveNum = 0;
  
	// ��������� �� �������� �� �������
    if ( getWinner() == m_turn )
	{
      // �� ������ ������ ������ ���-������ ��������� � ����
      // ���� ����� ������ ��� ������� ���� �� �������� �������
      _bestMove.moveFrom.x = 0;
      _bestMove.moveTo.x = 0;
      _bestMove.moveFrom.y = 0;
      _bestMove.moveTo.y = 0;

      return 100000 + _level;
	}

    // ������� �� ��������, ������ ������� ������
    if ( _level )
	{
      // ������� ������ ��������� �����
      list = getMoveList();
      // ���� ����������� - ��������� �����
      // ������� ������ Position
      P = new CornersAI( *this );
      // ������� ����� ������� �������
  
      //������������� ����� 
      curMove = list;
      BestScore = -100000;
      // ��� ������� ���������� ����:
      while ( curMove )
	  {
        // ���������� ���
        //P->makeMove( curMove->move );
		P->makeMove( curMove->move.moveFrom.x, curMove->move.moveFrom.y, curMove->move.moveTo.x, curMove->move.moveTo.y );
        // ������� GetBestMove ��� ���������� �������
        Score = P->getBestMove2( Move, _level - 1 );
        // O����������� ������ �� ��� ���
        if ( Score > BestScore )
		{
          BestScore = Score;
          _bestMove = curMove->move;
		}
        // ������� ��� �����
        //P->makeMove( curMove->move.moveTo, curMove->move.moveFrom );
		P->makeMove( curMove->move.moveTo.x, curMove->move.moveTo.y, curMove->move.moveFrom.x, curMove->move.moveFrom.y );

        // ��������� ���
        curMove = curMove->next;
	  }
      // ������� ������ ��������� �����
      delMoveList( list );
      // ���������� ������
      delete P;
      // ������� ������ ������� ����
      return BestScore;
	}
    else // ������ ������ ��������� �� _level
	{
      // ������� ������� �������
      // ������� �������� ������� �������
      return goalFunc2();
    }
}

int CornersAI::getWinner()
{
  bool NotWhite = false;
  bool NotBlack = false;
  int i;
  int j;
  
  // ��� ������
  for ( i = 0; i < 8; i++ )
  {
	  if ( m_initialBoard[i][0] != TState::White ) goto a;
      for ( j = 0; j < 8; j++ )
	  {
		  if ( m_initialBoard[i][j] = TState::White )
		  {
			  if ( m_board[i][j] != TState::Black )
			  {
				NotBlack = true;
				goto a;
			  }
		  }
		  else
		  {
				goto b; // C����� ����������
		  }
	  }
b: //������ �������� ���������� ����
	  ;
  }
a: //��� �����
  for ( i = 7; i > -1; i-- )
  {
	  if ( m_initialBoard[i][7] != TState::Black ) goto c;
      for ( j = 7; j > -1; j-- )
	  {
		  if ( m_initialBoard[i][j] == TState::Black )
		  {
			  if ( m_board[i][j] != TState::White )
			  {
				NotWhite = true;
				goto c;
			  }
		  }
          else
		  {
			goto d; //C����� ����������
		  }
	  }
d: //������ �������� ���������� ����
	  ;
  }
c: //��������� ��� ��������
  if ( NotWhite && NotBlack )
  {
	  return TState::Space;
  }
  else
  {
      if ( NotWhite )
	  {
		  return TState::Black;
	  }
	  else 
	  {
		  return TState::White;
	  }
  }
}
