
#include "ALSoundBuffer.h"
#include "ALSoundBufferStream.h"
#include "ALSoundSource.h"
#include "ALSoundSystem.h"

//#	include <Windows.h>
//////////////////////////////////////////////////////////////////////////
ALSoundSource::ALSoundSource(ALSoundSystem* _soundSystem) 
: m_soundBuffer( NULL )
, m_listener( NULL )
, m_sourceName( NULL )
, m_soundSystem( _soundSystem )
, m_busy( false )
, m_playing( false )
, m_looped( false )
, m_volume( 1.0f )
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
		m_soundBuffer->removeSource( this );
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::play()
{
	if( m_playing || !m_soundBuffer) 
	{
		return;
	}

	bool isStereo = m_soundBuffer->isStereo();
	m_sourceName = m_soundSystem->getFreeSourceName( isStereo );
	if( !m_sourceName ) 
	{
		//printf("no free sourceName or soundBuffer - returning\n");
		return;
	}

	m_sourceName->busy = true;

	_updateParams();
	if( m_soundBuffer && m_soundBuffer->isStreamed() )
	{
		alSourcei( m_sourceName->name, AL_BUFFER, NULL );
	    alSourcei( m_sourceName->name, AL_LOOPING, AL_FALSE ); //Streaming sources can't loop
		static_cast<ALSoundBufferStream*>( m_soundBuffer )
			->start( m_sourceName->name );
		m_soundSystem->addStream( static_cast<ALSoundBufferStream*>( m_soundBuffer ) );
	}
	else
	{
		//alSourcei( m_sourceName->name, AL_BUFFER, NULL );
		alSourcei( m_sourceName->name, AL_BUFFER, m_soundBuffer->getAlID() );
		//printf( "SoundSource::play\n" );
		alSourcePlay( m_sourceName->name );
	}

	//if( !m_looped /*&& !m_soundBuffer->isStreamed()*/ )
	{
		//printf("register playing %d ms\n", getLengthMs());
		int lengthMs = getLengthMs();
		m_soundSystem->registerPlaying( this, lengthMs );
	}

	m_playing = true;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::pause()
{
	alSourcePause( m_sourceName->name );

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

	if( !m_sourceName )
	{
		return;
	}

	if( m_soundBuffer && m_soundBuffer->isStreamed() )
	{
		static_cast<ALSoundBufferStream*>( m_soundBuffer )
			->stop();
		m_soundSystem->removeStream( static_cast<ALSoundBufferStream*>( m_soundBuffer ) );
	}
	else
	{
		//printf("stopping\n");
		alSourceStop( m_sourceName->name );
		alSourcei( m_sourceName->name, AL_BUFFER, NULL );
	}

	unbind();
	/*m_sourceName->busy = false;
	m_soundSystem->unregisterPlaying( this );

	if( m_busy && m_listener )
	{
		m_listener->listenStopped();
	}*/
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
	ALint pos;
	alGetSourcei( m_sourceName->name, AL_SEC_OFFSET, &pos );

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
	}
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::setVolume(float _volume)
{
	m_volume = _volume;

	if( m_sourceName && m_playing )
	{
		alSourcef( m_sourceName->name, AL_GAIN, m_volume );
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
		alSourcei( m_sourceName->name, AL_SOURCE_RELATIVE, AL_TRUE );
		alSourcef( m_sourceName->name, AL_ROLLOFF_FACTOR, 0.0f );
		alSource3f( m_sourceName->name, AL_DIRECTION, 0.0f, 0.0f, 0.0f );
	} 
	else 
	{
		alSourcei( m_sourceName->name, AL_SOURCE_RELATIVE, AL_FALSE );
		alSourcef( m_sourceName->name, AL_ROLLOFF_FACTOR, 1.0f );
	}

	if( m_soundBuffer && m_soundBuffer->isStreamed() )
	{
		//static_cast<ALSoundBufferStream*>( m_soundBuffer )
		//	->getUpdater()->setLooping( m_looped );
	}
	else
	{
		alSourcei( m_sourceName->name, AL_LOOPING, m_looped ? AL_TRUE : AL_FALSE );
	}

	alSourcefv( m_sourceName->name, AL_POSITION, m_position );
	alSourcef( m_sourceName->name, AL_GAIN, m_volume );
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
bool ALSoundSource::isBusy() const
{
	return m_busy;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::setUsed( bool _use )
{
	m_busy = _use;
}
//////////////////////////////////////////////////////////////////////////
Menge::SoundNodeListenerInterface* ALSoundSource::getListener()
{
	return m_listener;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::unbind()
{
	m_playing = false;
	if( m_sourceName != 0 )
	{
		m_sourceName->busy = false;
		m_soundSystem->unregisterPlaying( this );
	}
	if( m_busy && m_listener )
	{
		m_listener->listenStopped();
	}
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::onStreamEnd_()
{

}
//////////////////////////////////////////////////////////////////////////
