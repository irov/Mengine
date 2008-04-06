#	include "AstralaxEmitter.h"
#	include <assert.h>

//////////////////////////////////////////////////////////////////////////
AstralaxEmitter::AstralaxEmitter( HM_EMITTER _id )
	: m_id( _id )
	, m_start( false )
	, m_leftBorder( 0.0f )
	, m_total_rate( 0.0f )
	, m_looped( false )
	, m_listener( NULL )
{
	HM_EMITTER duplicated_id;
	Magic_DuplicateEmitter( m_id, &duplicated_id );
	m_typesCount = Magic_GetParticlesTypeCount( m_id );
	assert( m_typesCount < 20 && "Particles type count over 20!" );
	for( int i = 0; i < m_typesCount; i++ )
	{
		m_factor[i] = Magic_GetDiagramFactor( m_id, i, MAGIC_DIAGRAM_NUMBER );
	}
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

	for( int i = 0; i < m_typesCount; i++ )
	{
		Magic_SetDiagramFactor( m_id, i, MAGIC_DIAGRAM_NUMBER, m_factor[i] );
	}
	//m_factor = Magic_GetDiagramFactor( m_id, 0, MAGIC_DIAGRAM_NUMBER );
	//Magic_SetPosition( m_id, 0 );

	/*if( m_leftBorder > 0 )
	{
		_leftVisibleInterval( m_leftBorder );
	}
*/
	m_start = true;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::setLooped( int _loop )
{
	Magic_SetLoopMode( m_id, _loop );
}

int AstralaxEmitter::getLooped() const
{
	return Magic_GetLoopMode( m_id );
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::stop()
{
	//m_start = false;

	//Magic_Stop( m_id );
	//Magic_SetDiagramFactor( m_id, 0, 3, 0.0f );
	for( int i = 0; i < m_typesCount; i++ )
	{
		Magic_SetDiagramFactor( m_id, i, MAGIC_DIAGRAM_NUMBER, 0.0f );
	}
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
			m_start = false;
			if( m_listener )
			{
				m_listener->onStopped();
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
bool	AstralaxEmitter::isIntensive() const
{
	return Magic_IsIntensive();
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::setListener( ParticleEmitterListenerInterface* _listener )
{
	m_listener = _listener;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxEmitter::setPosition(float _x, float _y)
{
	Magic_SetEmitterPosition( m_id, int(_x), int(_y) );
}
//////////////////////////////////////////////////////////////////////////