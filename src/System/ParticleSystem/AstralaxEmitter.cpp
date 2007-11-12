#	include "AstralaxEmitter.h"
//////////////////////////////////////////////////////////////////////////
Emitter::Emitter( HM_EMITTER _id )
	: m_id( _id )
	, m_pause( true )
{
}
//////////////////////////////////////////////////////////////////////////
void Emitter::_leftVisibleInterval()
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
void	Emitter::play( bool _leftVisible )
{
	if( _leftVisible )
	{
		_leftVisibleInterval();
	}

	m_pause = false;
}
//////////////////////////////////////////////////////////////////////////
void	Emitter::stop()
{
	Magic_Stop( m_id );

	m_pause = true;
}
//////////////////////////////////////////////////////////////////////////
void	Emitter::update()
{
	if( m_pause )
	{
		return;
	}

	double rate = Magic_GetUpdateTime( m_id );
	Magic_Update( m_id, rate );
}
//////////////////////////////////////////////////////////////////////////
bool	Emitter::isPlaying() const
{
	return !m_pause;
}
//////////////////////////////////////////////////////////////////////////
int		Emitter::getParticlesTypeCount() const
{
	return Magic_GetParticlesTypeCount( m_id );
}
//////////////////////////////////////////////////////////////////////////
HM_EMITTER	Emitter::getId() const
{
	return m_id;
}
//////////////////////////////////////////////////////////////////////////