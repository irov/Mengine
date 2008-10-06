
#include "ALSoundBuffer.h"
#include "ALSoundBufferStream.h"
#include "ALSoundSource.h"
#include "ALSoundSystem.h"

//////////////////////////////////////////////////////////////////////////
ALSoundSource::ALSoundSource( ALSoundSystem* _soundSystem ) 
: m_soundBuffer( 0 )
, m_listener( 0 )
, m_alSource( 0 )
, m_soundSystem( _soundSystem )
, m_playing( false )
, m_looped( false )
, m_volume( 1.0f )
, m_counter( 0.0f )
{
	m_position[0] = 0.0f;
	m_position[1] = 0.0f;
	m_position[2] = 0.0f;
}
//////////////////////////////////////////////////////////////////////////
ALSoundSource::~ALSoundSource()
{
	if( m_playing )
	{
		stop();
	}
	if( m_soundBuffer )
	{
		m_soundBuffer->removeSource( this );
	}
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::play()
{
	if( m_playing || !m_soundBuffer) 
	{
		return;
	}

	m_playing = true;
	bool isStereo = m_soundBuffer->isStereo();

	m_alSource = m_soundSystem->registerPlaying( this, isStereo );
	if( m_alSource == 0 ) 
	{
		//MENGE_LOG_WARNING( "Warning: Out of hardware sound sources" );
		return;
	}

	_updateParams();

	if( m_soundBuffer->isStreamed() )
	{
		alSourcei( m_alSource, AL_BUFFER, NULL );
	    alSourcei( m_alSource, AL_LOOPING, AL_FALSE ); //Streaming sources can't loop
		static_cast<ALSoundBufferStream*>( m_soundBuffer )
			->start( m_alSource );
	}
	else
	{
		alSourcei( m_alSource, AL_BUFFER, m_soundBuffer->getAlID() );
		alSourcePlay( m_alSource );
	}
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::pause()
{
	alSourcePause( m_alSource );

	m_soundSystem->unregisterPlaying( this, m_alSource );

	if( m_listener )
	{
		m_listener->listenPaused( true );
	}
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::stop()
{
	if( !m_playing ) return;
	m_playing = false;

	if( m_alSource != 0 )
	{
		alSourceStop( m_alSource );
		alSourcei( m_alSource, AL_BUFFER, NULL );
	}

	if( m_soundBuffer && m_soundBuffer->isStreamed() )
	{
		static_cast<ALSoundBufferStream*>( m_soundBuffer )->stop();
	}

	m_soundSystem->unregisterPlaying( this, m_alSource );
	m_alSource = 0;
	m_counter = getLengthMs();
	if( m_listener )
	{
		m_listener->listenStopped();
	}
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundSource::isPlaying() const
{
	return m_playing;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::setPosition( float _x, float _y, float _z )
{
	m_position[0] = _x;
	m_position[1] = _y;
	m_position[2] = _z;
}
//////////////////////////////////////////////////////////////////////////
const float * ALSoundSource::getPosition() const
{
	return m_position;
}
//////////////////////////////////////////////////////////////////////////
int ALSoundSource::getLengthMs()
{
	if( m_soundBuffer )
	{
		return m_soundBuffer->getLenghtMs();
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
int ALSoundSource::getPosMs()
{
	ALint pos = 0;

	if( m_alSource == 0 )
	{
		alGetSourcei( m_alSource, AL_SEC_OFFSET, &pos );
	}
	return pos;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::setSoundBuffer( Menge::SoundBufferInterface* _soundBuffer )
{
	m_soundBuffer = static_cast<ALSoundBuffer*>( _soundBuffer );
	if( m_soundBuffer )
	{
		m_soundBuffer->removeSource( this );
		m_soundBuffer->addSource( this );
		m_counter = m_soundBuffer->getLenghtMs();
	}
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::setVolume( float _volume )
{
	m_volume = _volume;

	if( m_alSource && m_playing )
	{
		alSourcef( m_alSource, AL_GAIN, m_volume );
	}
}
//////////////////////////////////////////////////////////////////////////
float ALSoundSource::getVolume() const
{
	return m_volume;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::_updateParams()
{
	if(m_ambient)
	{
		alSourcei( m_alSource, AL_SOURCE_RELATIVE, AL_TRUE );
		alSourcef( m_alSource, AL_ROLLOFF_FACTOR, 0.0f );
		alSource3f( m_alSource, AL_DIRECTION, 0.0f, 0.0f, 0.0f );
	} 
	else 
	{
		alSourcei( m_alSource, AL_SOURCE_RELATIVE, AL_FALSE );
		alSourcef( m_alSource, AL_ROLLOFF_FACTOR, 1.0f );
	}

	if( m_soundBuffer && m_soundBuffer->isStreamed() == false )
	{
		alSourcei( m_alSource, AL_LOOPING, m_looped ? AL_TRUE : AL_FALSE );	
	}

	alSourcefv( m_alSource, AL_POSITION, m_position );
	alSourcef( m_alSource, AL_GAIN, m_volume );
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::setLooped( bool _loop )
{
	m_looped = _loop;
	if( m_playing )
	{
		_updateParams();
	}
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundSource::isLooping() const
{
	return m_looped;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::setSoundNodeListener( Menge::SoundNodeListenerInterface* _listener )
{
	m_listener = _listener;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::setAmbient( bool _ambient )
{
	m_ambient = _ambient;
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundSource::isAmbient() const
{
	return m_ambient;
}
//////////////////////////////////////////////////////////////////////////
Menge::SoundNodeListenerInterface* ALSoundSource::getListener()
{
	return m_listener;
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundSource::update( float _timing )
{
	//if( m_playing == false ) return;

	if( m_soundBuffer->isStreamed() )
	{
		((ALSoundBufferStream*)m_soundBuffer)->update();
	}
	if( m_looped == false || m_soundBuffer->isStreamed() )
	{
		m_counter -= _timing;
		if( m_counter <= 0.0f )
		{
			return false;
			//m_soundSystem->unregisterPlaying( this, m_alSource );
			//stop();
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::_onStop()
{
	if( !m_playing ) return;
	m_playing = false;

	if( m_alSource != 0 )
	{
		alSourceStop( m_alSource );
		alSourcei( m_alSource, AL_BUFFER, NULL );
	}

	if( m_soundBuffer && m_soundBuffer->isStreamed() )
	{
		static_cast<ALSoundBufferStream*>( m_soundBuffer )->stop();
	}

	m_alSource = 0;
	m_counter = getLengthMs();
	if( m_listener )
	{
		m_listener->listenStopped();
	}
}
//////////////////////////////////////////////////////////////////////////
