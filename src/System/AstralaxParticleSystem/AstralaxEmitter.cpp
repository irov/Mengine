#	include "AstralaxEmitter.h"
#	include <assert.h>
//////////////////////////////////////////////////////////////////////////
AstralaxEmitter::AstralaxEmitter( HM_EMITTER _id )
	: m_id( _id )
	, m_start( false )
	, m_leftBorder( 0.0f )
	, m_total_rate( 0.0f )
	, m_looped( false )
	//, m_stop( false )
{
	HM_EMITTER duplicated_id;
	Magic_DuplicateEmitter( m_id, &duplicated_id );
	m_id = duplicated_id;
}
//////////////////////////////////////////////////////////////////////////
AstralaxEmitter::~AstralaxEmitter()
{
//	Magic_UnloadEmitter( m_id );
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::getBoundingBox( int & left, int & top, int & right, int & bottom )  const
{
	Magic_GetRect( m_id, &left, &top, &right, &bottom );
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::setLeftBorder( float _leftBorder )
{
	m_leftBorder = _leftBorder;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::play()
{
	if ( Magic_IsRestart( m_id ) ) 
	{ 
		Magic_Restart( m_id );
	}

	//Magic_SetPosition( m_id, 0 );

	if( m_leftBorder > 0 )
	{
		_leftVisibleInterval( m_leftBorder );
	}

	m_start = true;

//	m_stop = false;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::setLooped( bool _loop )
{
	//int mode = _loop ? MAGIC_LOOP : MAGIC_NOLOOP;
	//Magic_SetLoopMode( m_id, mode );
	m_looped = _loop;
}

bool AstralaxEmitter::getLooped() const
{
	return m_looped;//Magic_GetLoopMode( m_id ) == MAGIC_LOOP;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::stop()
{
	m_start = false;

	Magic_Stop( m_id );

	Magic_Restart( m_id );

	//m_stop = true;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::pause()
{
	m_start = false;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::update( float _timing )
{
	if( m_start == false )
	{
		return;
	}

	m_total_rate += _timing;

    double rate = Magic_GetUpdateTime( m_id );

    while( m_total_rate >= rate )
    {
		m_total_rate -= rate;
        Magic_Update( m_id, rate );

        if ( Magic_IsRestart( m_id ) )
        { 
			if( m_looped /*&& m_stop == false*/ )
			{
				Magic_Restart( m_id );
			}
			else
			{
				//if( m_stop == true )
				//{
					m_start = false;
				//}
			}
        }
    }
}
//////////////////////////////////////////////////////////////////////////
int	AstralaxEmitter::getNumTypes() const
{
	return Magic_GetParticlesTypeCount( m_id );
}
//////////////////////////////////////////////////////////////////////////
HM_EMITTER AstralaxEmitter::getId() const
{
	return m_id;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::_leftVisibleInterval( double _left )
{
	//double left_interval = Magic_GetInterval1( m_id );

	//Magic_SetUpdateTemp( m_id, 0.5f );

	double duration = Magic_GetDuration( m_id );

	if( _left >= duration )
	{
		return;
	}

	double position = Magic_GetPosition( m_id );

	if( _left > 0)
	{
		while( position < _left )
		{
			position = Magic_GetPosition( m_id );

			double rate = Magic_GetUpdateTime( m_id );

			float temp = Magic_GetUpdateTemp( m_id );

			if (position + rate * temp > _left)
			{
				rate = ( _left - position ) / temp;
			}

			Magic_Update( m_id, rate );
		}

		HM_DIMENSION to;

		if ( Magic_CopyDimensionTo( m_id, &to ) == MAGIC_SUCCESS )
		{
			Magic_SetDimensionID( m_id, to );
		}
	}
}
//////////////////////////////////////////////////////////////////////////