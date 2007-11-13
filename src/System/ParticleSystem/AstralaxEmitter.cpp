#	include "AstralaxEmitter.h"
#	include <assert.h>
//////////////////////////////////////////////////////////////////////////
AstralaxEmitter::AstralaxEmitter( HM_EMITTER _id )
	: m_id( _id )
	, m_start( false )
{
}
//////////////////////////////////////////////////////////////////////////
AstralaxEmitter::~AstralaxEmitter()
{
	Magic_UnloadEmitter( m_id );
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::getBoundingBox( int & left, int & top, int & right, int & bottom )  const
{
	Magic_GetRect( m_id, &left, &top, &right, &bottom );
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::play( bool _leftVisible )
{
	if( _leftVisible )
	{
		_leftVisibleInterval();
	}

	if ( Magic_IsRestart( m_id ) ) 
	{ 
		Magic_Restart( m_id );
	}

	m_start = true;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::setLooped( bool _loop )
{
	int mode = _loop ? MAGIC_LOOP : MAGIC_NOLOOP;
	Magic_SetLoopMode( m_id, mode );
}

bool AstralaxEmitter::getLooped() const
{
	return Magic_GetLoopMode( m_id ) == MAGIC_LOOP;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::stop()
{
	m_start = false;

	Magic_Stop( m_id );

	Magic_Restart( m_id );
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

	//double rate = Magic_GetUpdateTime( m_id );

	Magic_Update( m_id, _timing );

	if ( Magic_IsRestart( m_id ) ) 
	{ 
		m_start = false;
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
void AstralaxEmitter::_leftVisibleInterval()
{
	double left_interval = Magic_GetInterval1( m_id );

	double position = Magic_GetPosition( m_id );

	if( left_interval > 0)
	{
		while( position < left_interval )
		{
			position = Magic_GetPosition( m_id );

			double rate = Magic_GetUpdateTime( m_id );

			float temp = Magic_GetUpdateTemp( m_id );

			if (position + rate * temp > left_interval)
			{
				rate = (left_interval - position) / temp;
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