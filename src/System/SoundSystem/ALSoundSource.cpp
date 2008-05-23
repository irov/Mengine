
#include "ALSoundBuffer.h"
#include "ALSoundBufferStream.h"
#include "ALSoundSource.h"
#include "ALSoundSystem.h"

//////////////////////////////////////////////////////////////////////////
void CALLBACK ListenStoppedCallback(void* _source, BOOL)
{
	ALSoundSource* source = static_cast<ALSoundSource*>(_source);
	SoundNodeListenerInterface* listener = source->getListener();
	if( source->isPlaying() && !source->isLooping() )
	{
		source->stop();
	}
	/*if(source->isPlaying() && listener)
	{
		listener->listenStopped();
	}*/
}
//////////////////////////////////////////////////////////////////////////
ALSoundSource::ALSoundSource(ALSoundSystem* _soundSystem) 
:m_soundBuffer( NULL )
,m_listener( NULL )
,m_sourceName( NULL )
,m_soundSystem( _soundSystem )
,m_busy( false )
,m_playing( false )
,m_looped( false )
,m_volume( 1.0f )
,m_stopCallbackHandle( NULL )
{
	ZeroMemory( m_position, sizeof( float ) * 3 );
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
	if( m_playing ) 
	{
		//printf("already playing - returning\n");
		return;
	}

	m_sourceName = m_soundSystem->getFreeSourceName();
	if(!m_sourceName || !m_soundBuffer) 
	{
		//printf("no free sourceName or soundBuffer - returning\n");
		return;
	}

	m_sourceName->busy = true;

	_updateParams();

	if( m_soundBuffer && m_soundBuffer->isStreamed() )
	{
		//alSourcei( m_sourceName->name, AL_BUFFER, NULL );
	    alSourcei( m_sourceName->name, AL_LOOPING, AL_FALSE ); //Streaming sources can't loop
		static_cast<ALSoundBufferStream*>( m_soundBuffer )
			->record( m_sourceName->name );
		m_soundSystem->addStream( static_cast<ALSoundBufferStream*>( m_soundBuffer ) );
	}
	else
	{
		//::Sleep( 100 );
		//printf("playing %s\n", m_soundBuffer->getFilename().c_str() );
		//alSourcei( m_sourceName->name, AL_BUFFER, NULL );
		alSourcei( m_sourceName->name, AL_BUFFER, m_soundBuffer->getBufferName() );
		if( alGetError() != AL_FALSE ) printf("ALERROR!\n");
		alSourcePlay( m_sourceName->name );
		if( alGetError() != AL_FALSE ) printf("ALERROR!\n");
	}

	if( !m_looped /*&& !m_soundBuffer->isStreamed()*/ )
	{
		//printf("register playing %d ms\n", getLengthMs());
		m_soundSystem->registerPlaying( this, getLengthMs() );
	}
	//CreateTimerQueueTimer( &m_stopCallbackHandle, NULL, (WAITORTIMERCALLBACK)ListenStoppedCallback, this, getLengthMs(), 0, WT_EXECUTEONLYONCE );
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
	//printf("stopping\n");

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
		alSourceStop( m_sourceName->name );
		alSourcei( m_sourceName->name, AL_BUFFER, NULL );
	}

	m_sourceName->busy = false;
	m_soundSystem->unregisterPlaying( this );
	//DeleteTimerQueueTimer( NULL, m_stopCallbackHandle, NULL );

	if( m_busy && m_listener )
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
	ALint pos;
	alGetSourcei( m_sourceName->name, AL_SEC_OFFSET, &pos );

	return pos;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSource::setSoundBuffer( SoundBufferInterface* _soundBuffer )
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
		static_cast<ALSoundBufferStream*>( m_soundBuffer )
			->getUpdater()->setLooping( m_looped );
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
void ALSoundSource::setSoundNodeListener( SoundNodeListenerInterface* _listener )
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
SoundNodeListenerInterface* ALSoundSource::getListener()
{
	return m_listener;
}
//////////////////////////////////////////////////////////////////////////