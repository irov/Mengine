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
	TMoveList* list = NULL;	// Указатель на результирующий список
	TMovesTree* tree = NULL;	// Указатель на "голову" дерева
	int x;
	int y;
	list = NULL;
	// Для каждой шашки текущего цвета
	for( x = 0; x < 8; x++ )
	{
		for( y = 0; y < 8; y++ )
		{
			if( m_board[x][y] == m_turn )
			{
				// Построить дерево
				createTree( &tree, NULL, x, y );

				// Добавить в список
				treeToList( tree, tree, &list );

				// Уничтожить дерево
				deleteTree( tree );

			}
		}
	}
	return list;
}

void CornersAI::createTree( TMovesTree** _head, TMovesTree* _parent, int _x, int _y, bool _isSimple /* = true  */ )
{
	// Инициализация
	(*_head) = new TMovesTree( _parent, _x, _y );
	// Просмотр возможных продолжений хода
	if (_isSimple)
	{
		// Простые шаги обрабатываются только для первого шага
		// Рекурсия не вызывается, а просто создаются конечные узлы
		if ( _x != 7 && m_board[_x+1][_y] == Space )
		{
			(*_head)->simpleStep[Right] = new TMovesTree( (*_head), _x+1, _y );
			//initialize( &(*_head)->simpleStep[TDirection::Right], (*_head), _x+1, _y );
		}
		if ( _x != 0 && m_board[_x-1][_y] == Space )
		{
			(*_head)->simpleStep[Left] = new TMovesTree( (*_head), _x-1, _y );
			//initialize( &(*_head)->simpleStep[TDirection::Left], (*_head), _x-1, _y );
		}
		if ( _y != 7 && m_board[_x][_y+1] == Space )
		{
			(*_head)->simpleStep[Up] = new TMovesTree( (*_head), _x, _y+1 );
			//initialize( &(*_head)->simpleStep[TDirection::Up], (*_head), _x, _y+1 );
		}
		if ( _y != 0 && m_board[_x][_y-1] == Space )
		{
			(*_head)->simpleStep[Down] = new TMovesTree( (*_head), _x, _y-1 );
			//initialize( &(*_head)->simpleStep[TDirection::Down], (*_head), _x, _y-1 );
		}
	}
	// Обрабатывается для любого шага - прыжки
	// Рекурсивно вызывается CreateTree
	// Вправо
	if ( _x != 7 
		&& _x+1 != 7 
		&& m_board[_x+1][_y] != Space 
		&& m_board[_x+2][_y] == Space 
		&& isFirstTime( (*_head), _x+2, _y ) )
	{
		createTree( &(*_head)->jumpStep[Right], (*_head), _x+2, _y, false );
	}
	// Влево
	if ( _x != 0 
		&& _x-1 != 0 
		&& m_board[_x-1][_y] != Space 
		&& m_board[_x-2][_y] == Space 
		&& isFirstTime( (*_head), _x-2, _y ) )
	{
		createTree( &(*_head)->jumpStep[Left], (*_head), _x-2, _y, false);
	}
	// Вверх
	if ( _y != 7 
		&& _y+1 != 7 
		&& m_board[_x][_y+1] != Space 
		&& m_board[_x][_y+2] == Space 
		&& isFirstTime( (*_head), _x, _y+2 ) )
	{
		createTree( &(*_head)->jumpStep[Up], (*_head), _x, _y+2, false );
	}
	// Вниз
	if ( _y != 0 
		&& _y-1 != 0 
		&& m_board[_x][_y-1] != Space 
		&& m_board[_x][_y-2] == Space 
		&& isFirstTime( (*_head), _x, _y-2 ) )
	{
		createTree( &(*_head)->jumpStep[Down], (*_head), _x, _y-2, false );
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

	// Проверить не является ли корнем, а если является, то пропустить добавление
	if ( _tree != _node )
	{
		// Проверить есть ли текущий ход в списке и если надо - добавить
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
			temp = new TMoveList;									//Создать новый элемент
			temp->move.moveFrom.x = _tree->disposition.x;			//проинициализировать
			temp->move.moveFrom.y = _tree->disposition.y;
			temp->move.moveTo.x = _node->disposition.x;
			temp->move.moveTo.y = _node->disposition.y;
			if ( (*_list) != NULL) 
			{
				(*_list)->prev = temp;                           // и внести в список первым
			}
			temp->next = (*_list);
			temp->prev = NULL;
			(*_list) = temp;
		}
	}
	// Рекрусивно вызвать себя для всех "листьев" дерева не равных нулю}
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
	// Вначале удалим подчиненные узлы
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
	//А теперь удалим сам узел
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
	// Подсчет расстояний
	for ( i = 0; i < 8; i++ )
	{
		for ( j = 0; j < 8; j++ )
		{
			if ( m_board[i][j] == White )
			{
				// Сумма расстояний до угла черных
				// WhiteR:=WhiteR+16-ord(i)-ord(j);
				WhiteR = WhiteR + ( 16 - MIN(i, 7) - MIN(j, 7) );
			}
			if ( m_board[i][j] == Black )
			{
				// Сумма расстояний до угла белых
				// BlackR:=BlackR+ord(i)+ord(j);
				BlackR = BlackR + ( MAX(i, 2) + MAX(j, 2) );
			}
		}
	}
	if ( m_turn == Black )
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
	// Подсчет расстояний
	for ( i = 0; i < 8; i++ )
	{
		for ( j = 0; j < 8; j++ )
		{
			if ( m_board[i][j] == White )
			{
				// Взвешенная Сумма расстояний до угла черных
				if ( i < 4 || j < 2 )
				{
					WhiteR = WhiteR + ( 16 - i - j )*5;
				}
				else
				{
					WhiteR = WhiteR + 16 - i - j;
				}
				// Количество достигших своего места
				if ( m_initialBoard[i][j] == Black )
				{
					WhiteCount++;
				}
			}
			else if ( m_board[i][j] == Black )
			{
				// Взвешенная Сумма расстояний до угла белых
				if ( i > 5 || j > 5)
				{
					BlackR = BlackR + ( i + j )*5;
				}
				else
				{
					BlackR = BlackR + i + j;
				}
				// Количество достигших своего места}
				if ( m_initialBoard[i][j] == White )
				{
					BlackCount++;
				}
			}
		}
	}

	if ( m_turn == Black )
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
	m_board[_x][_y] = Space;
}

void CornersAI::remvoePiece( const TPlace& _place )
{
	m_board[_place.x][_place.y] = Space;
}

void CornersAI::makeMove( int _x1, int _y1, int _x2, int _y2 )
{
	m_board[_x2][_y2] = m_board[_x1][_y1];
	m_board[_x1][_y1] = Space;
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
	// Посчитать сумму расстояний до целевого угла
	for ( i = 0; i < 8; i++ )
	{
		for ( j = 0; j < 8; j++ )
		{
			if ( m_board[i][j] == White )
			{
				WhiteR = WhiteR + 16 - i - j;
				Count++;
			}
			else if ( m_board[i][j] == Black )
			{
				BlackR = BlackR + i + j;
			}
			if( m_initialBoard[i][j] == White && m_board[i][j] != Black )
			{
				BlackSp++;
			}
			else if( m_initialBoard[i][j] == Black && m_board[i][j] != White )
			{
				WhiteSp++;
			}
		}
	}

	// Выбрать алгоритм вычисления следующего хода
	if ( ( m_turn == White && WhiteR > Count*5 ) || ( m_turn == Black && BlackR > Count*5) )
	{
		// Выбор хода по правилам первой половины партии
		getBestMove( Result, _level );
	}
	else
	{
		// Выбор хода по правилам второй половины партии
		if ( ( m_turn == White && WhiteSp <= 3 ) || ( m_turn == Black && BlackSp <= 3 ) )
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
		// Создать список возможных ходов
		list = getMoveList();
		// Если понадобится - посчитаем позже
		// Создать объект Position
		P = new CornersAI( *this );
		// Создать копию текущей позиции

		// Инициализация цикла
		curMove = list;
		BestScore = -1000000;
		// Для каждого возможного хода:
		while ( curMove )
		{
			//Произвести ход
			P->makeMove( curMove->move.moveFrom.x, curMove->move.moveFrom.y, curMove->move.moveTo.x, curMove->move.moveTo.y );
			// Передать ход сопернику
			P->changeTurn();
			// Вызвать GetBestMove для полученной позиции
			Score = -P->getBestMove( Move, _level - 1);
			// Oпределиться лучший ли ход был
			if ( Score > BestScore )
			{
			  BestScore = Score;
			  _bestMove = curMove->move;
			}
			// Вернуть ход назад
			//P->makeMove( curMove->move.moveTo, curMove->move.moveFrom );
			P->makeMove( curMove->move.moveTo.x, curMove->move.moveTo.y, curMove->move.moveFrom.x, curMove->move.moveFrom.y );
			P->changeTurn();
			// Следующий ход
			curMove = curMove->next;
		}
		// Удалить список возможных ходов}
		delMoveList( list );
		// Освободить объект}
		delete P;
		// Вернуть оценку лучшего хода
		return BestScore;
	}
	else // Прошли дерево просмотра до _level
	{
	  // вызвать целевую функцию
	  // вернуть значение целевой функции
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
  
	// Проверить не конечная ли позиция
    if ( getWinner() == m_turn )
	{
      // На всякий случай вернем что-нибудь приличное в ходе
      // Хотя нужно только при попытке хода из конечной позиции
      _bestMove.moveFrom.x = 0;
      _bestMove.moveTo.x = 0;
      _bestMove.moveFrom.y = 0;
      _bestMove.moveTo.y = 0;

      return 100000 + _level;
	}

    // Позиция не конечная, значит считаем дальше
    if ( _level )
	{
      // Создать список возможных ходов
      list = getMoveList();
      // Если понадобится - посчитаем позже
      // Создать объект Position
      P = new CornersAI( *this );
      // Создать копию текущей позиции
  
      //Инициализация цикла 
      curMove = list;
      BestScore = -1000000;
      // Для каждого возможного хода:
      while ( curMove )
	  {
        // Произвести ход
        //P->makeMove( curMove->move );
		P->makeMove( curMove->move.moveFrom.x, curMove->move.moveFrom.y, curMove->move.moveTo.x, curMove->move.moveTo.y );
        // Вызвать GetBestMove для полученной позиции
        Score = P->getBestMove2( Move, _level - 1 );
        // Oпределиться лучший ли ход был
        if ( Score > BestScore )
		{
          BestScore = Score;
          _bestMove = curMove->move;
		}
        // Вернуть ход назад
        //P->makeMove( curMove->move.moveTo, curMove->move.moveFrom );
		P->makeMove( curMove->move.moveTo.x, curMove->move.moveTo.y, curMove->move.moveFrom.x, curMove->move.moveFrom.y );

        // Следующий ход
        curMove = curMove->next;
	  }
      // Удалить список возможных ходов
      delMoveList( list );
      // Освободить объект
      delete P;
      // Вернуть оценку лучшего хода
      return BestScore;
	}
    else // Прошли дерево просмотра до _level
	{
      // вызвать целевую функцию
      // вернуть значение целевой функции
      return goalFunc2();
    }
}

int CornersAI::getWinner()
{
  bool NotWhite = false;
  bool NotBlack = false;
  int i;
  int j;
  
  // Для черных
  for ( i = 0; i < 8; i++ )
  {
	  if ( m_initialBoard[i][0] != White ) goto a;
      for ( j = 0; j < 8; j++ )
	  {
		  if ( m_initialBoard[i][j] = White )
		  {
			  if ( m_board[i][j] != Black )
			  {
				NotBlack = true;
				goto a;
			  }
		  }
		  else
		  {
				goto b; // Cтроку досмотрели
		  }
	  }
b: //Просто прервали внутренний цикл
	  ;
  }
a: //Для белых
  for ( i = 7; i > -1; i-- )
  {
	  if ( m_initialBoard[i][7] != Black ) goto c;
      for ( j = 7; j > -1; j-- )
	  {
		  if ( m_initialBoard[i][j] == Black )
		  {
			  if ( m_board[i][j] != White )
			  {
				NotWhite = true;
				goto c;
			  }
		  }
          else
		  {
			goto d; //Cтроку досмотрели
		  }
	  }
d: //Просто прервали внутренний цикл
	  ;
  }
c: //Посмотрим что получили
  if ( NotWhite && NotBlack )
  {
	  return Space;
  }
  else
  {
      if ( NotWhite )
	  {
		  return Black;
	  }
	  else 
	  {
		  return White;
	  }
  }
}
