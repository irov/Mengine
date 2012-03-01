
#	include "OALSoundSystem.h"
#	include "Interface/LogSystemInterface.h"
#	include "Config/Config.h"
#	include "SulkSystem.h"

#	include "OALError.h"
#	include "OALSoundBufferBase.h"
#	include "OALSoundBufferStream.h"
#	include "OALSoundSource.h"

#	include "Logger/Logger.h"

#	include <algorithm>
#	include <stdio.h>
#	include <stdarg.h>

#	define OAL_CHECK_ERROR() s_OALErrorCheck( this, __FILE__, __LINE__ )

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::SoundSystemInterface** _interface )
{
	if( _interface == 0 )
	{
		return false;
	}

	*_interface = new Menge::OALSoundSystem();

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::SoundSystemInterface* _interface )
{
	if( _interface )
	{
		delete static_cast<Menge::OALSoundSystem*>( _interface );
	}
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
#	define MAX_SOUND_SOURCES 32
#	define MAX_SOUND_BUFFERS 16

	//////////////////////////////////////////////////////////////////////////
	OALSoundSystem::OALSoundSystem()
		: m_logSystem(NULL)
		, m_initialized(false)
		, m_context(NULL)
		, m_device(NULL)
		, m_sulk(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundSystem::~OALSoundSystem()
	{
		if( m_sulk != NULL )
		{
			delete m_sulk;
			m_sulk = NULL;
		}

		//alDeleteSources( m_sourcePool.size(), &(m_sourcePool[0]) );
		//m_sourcePool.clear();

		//alDeleteBuffers( m_bufferPool.size(), &(m_bufferPool[0]) );
		//m_bufferPool.clear();

		alcMakeContextCurrent( NULL );

		if( m_context )
		{
			alcDestroyContext( m_context );
			m_context = NULL;
		}

		if( m_device )
		{
			alcCloseDevice( m_device );
			m_device = NULL;
		}

		m_initialized = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSystem::initialize( LogSystemInterface* _logSystem )
	{
		if( m_initialized == true )
		{
			LOGGER_ERROR(m_logSystem)( "OALSoundSystem: system have been already initialized" );
			return false;
		}
		
		m_logSystem = _logSystem;
		LOGGER_INFO(m_logSystem)( "Starting OpenAL Sound System..." );

		//const ALCchar *devices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);//

		//MENGE_LOG_ERROR( "OALSoundSystem.initialize: devices '%s'"
		//	, devices
		//	);
				
		// open default device
		
		m_device = alcOpenDevice("Generic Software");
		OAL_CHECK_ERROR();
				
		if( m_device == NULL )
		{
			LOGGER_ERROR(m_logSystem)( "OALSoundSystem.initialize: Failed to open 'generic software' sound device try default..." );

			//const ALCchar* str = alcGetString( NULL, ALC_DEVICE_SPECIFIER );
			// Открываем программное устройство
			//m_device = alcOpenDevice("Generic Software");
			m_device = alcOpenDevice( NULL );
			OAL_CHECK_ERROR();

			if( m_device == NULL )
			{
				LOGGER_ERROR(m_logSystem)( "OALSoundSystem.initialize: Failed to open default sound device try hardware" );
			}

			// open hardware device
			m_device = alcOpenDevice( "Generic Hardware" );	
			OAL_CHECK_ERROR();

			if( m_device == NULL )
			{
				LOGGER_ERROR(m_logSystem)( "OALSoundSystem.initialize: Failed to open hardware sound device.." );

				return false;
			}			
		}

		m_context = alcCreateContext( m_device, NULL );
		OAL_CHECK_ERROR();

		if( m_context == NULL )
		{
			LOGGER_ERROR(m_logSystem)( "OALSoundSystem: Failed to create context" );
			alcCloseDevice( m_device );
			m_device = NULL;
			return false;
		}

		alcMakeContextCurrent( m_context );
		OAL_CHECK_ERROR();

		LOGGER_INFO(m_logSystem)( "OpenAL driver properties" );

		LOGGER_INFO(m_logSystem)( "Version: " );
		LOGGER_INFO(m_logSystem)( alGetString( AL_VERSION ) );

		LOGGER_INFO(m_logSystem)( "Vendor: " );
		LOGGER_INFO(m_logSystem)( alGetString( AL_VENDOR ) );

		LOGGER_INFO(m_logSystem)( "Renderer: " );
		LOGGER_INFO(m_logSystem)( alGetString( AL_RENDERER ) );

		if( alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE )
		{
			LOGGER_INFO(m_logSystem)( "Device Specifier: " );
			LOGGER_INFO(m_logSystem)( alcGetString( m_device, ALC_DEVICE_SPECIFIER ) );
		}
		//LOG( alGetString( AL_EXTENSIONS ) );

		float lposition[] = { 0.0f, 0.0f, 0.0f };
		float lvelocity[] = { 0.0f, 0.0f, 0.0f };
		float lorient[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
		alListenerfv( AL_POSITION, lposition );
		alListenerfv( AL_VELOCITY, lvelocity );
		alListenerfv( AL_ORIENTATION, lorient );

		//ALuint sourcePool[MAX_SOUND_SOURCES];
		//alGenSources( MAX_SOUND_SOURCES, sourcePool );
		//OAL_CHECK_ERROR();

		//m_sourcePool.assign( sourcePool, sourcePool + MAX_SOUND_SOURCES );

		//ALuint bufferPool[MAX_SOUND_BUFFERS];
		//alGenBuffers( MAX_SOUND_BUFFERS, bufferPool );
		//OAL_CHECK_ERROR();

		//m_bufferPool.assign( bufferPool, bufferPool + MAX_SOUND_BUFFERS );
		
		m_sulk = new SulkSystem();

		m_initialized = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::onTurnSound( bool _turn )
	{
		//if( _turn == false )
		//{
		//	alcMakeContextCurrent( NULL );
		//	OAL_CHECK_ERROR();

		//	alcSuspendContext( m_context );
		//	OAL_CHECK_ERROR();
		//}
		//else
		//{
		//	alcMakeContextCurrent( m_context );
		//	OAL_CHECK_ERROR();

		//	alcProcessContext( m_context );
		//	OAL_CHECK_ERROR();
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	LogSystemInterface * OALSoundSystem::getLogSystem() const
	{
		return m_logSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::setListenerOrient( const float* _position, const float* _front, const float* _top )
	{
		if( m_initialized == false )
		{
			return;
		}

		float orient[] = { _front[0], _front[1], _front[2], _top[0], _top[1], _top[2] };
		alListenerfv( AL_POSITION, _position );
		OAL_CHECK_ERROR();

		alListenerfv( AL_ORIENTATION, orient );
		OAL_CHECK_ERROR();
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterface* OALSoundSystem::createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample )
	{
		//OALSoundSource* soundSource = m_soundSources.get();
		OALSoundSource* soundSource = new OALSoundSource;

		soundSource->initialize(this);
		
		soundSource->setHeadMode( _isHeadMode );
		soundSource->loadBuffer( _sample );

		return soundSource;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface* OALSoundSystem::createSoundBuffer( SoundDecoderInterface* _soundDecoder, bool _isStream )
	{
		OALSoundBufferBase* buffer = NULL;
		if( m_initialized == false )
		{
			buffer = new OALSoundBufferBase();
		}
		else
		{
			if( _isStream == false )
			{
				buffer = new OALSoundBuffer( this );
			}
			else
			{
				buffer = new OALSoundBufferStream( this );
			}
		}

		if( buffer->load( _soundDecoder ) == false )
		{
			LOGGER_ERROR(m_logSystem)( "OALSoundSystem: Failed to create sound buffer from stream" 
				);

			buffer->release();
			buffer = NULL;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	//SoundBufferInterface* OALSoundSystem::createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem )
	//{

	//	return NULL;
	//}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::releaseSoundBuffer( SoundBufferInterface * _soundBuffer )
	{
		if( _soundBuffer != NULL )
		{
			_soundBuffer->release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::releaseSoundNode( SoundSourceInterface * _sn )
	{
		OALSoundSource* soundSource = static_cast<OALSoundSource*>( _sn );

		//m_soundSources.release(soundSource);
		delete soundSource;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSystem::setBlow( bool _active )
	{
		if( m_sulk )
		{
			return m_sulk->activate( _active );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundSystem::getBlow()
	{
		if( m_sulk )
		{
			return m_sulk->getBlow();
		}

		return 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::setEnoughBlow( float _enoughBlow )
	{
		if( m_sulk )
		{
			m_sulk->setEnoughBlow( _enoughBlow );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::setBlowCallback( SoundSulkCallbackInterface * _callback )
	{
		if( m_sulk )
		{
			m_sulk->setCallback( _callback );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::update( float _timing )
	{
		if( m_sulk )
		{
			m_sulk->update();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ALuint OALSoundSystem::genSourceId()
	{
		ALuint sourceId = 0;
		alGenSources( 1, &sourceId );

		return sourceId;

		//if( m_sourcePool.empty() == true )
		//{
		//	return 0;
		//}

		//ALuint sourceId = m_sourcePool.back();
		//m_sourcePool.pop_back();

		//return sourceId;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::releaseSourceId( ALuint _sourceId )
	{
		alDeleteSources( 1, &_sourceId );

		//ALint state = 0;
		//alGetSourcei( _sourceId, AL_SOURCE_STATE, &state );

		//if( state != AL_STOPPED && state != AL_INITIAL )
		//{
		//	this->clearSourceId( _sourceId );
		//}
		//
		//alSourcei( _sourceId, AL_BUFFER, 0 );
		//
		//m_sourcePool.push_back( _sourceId );
	}
	//////////////////////////////////////////////////////////////////////////
	ALuint OALSoundSystem::genBufferId()
	{
		ALuint bufferId = 0;
		alGenBuffers( 1, &bufferId );

		return bufferId;

		//if( m_bufferPool.empty() == true )
		//{
		//	return 0;
		//}

		//ALuint bufferId = m_bufferPool.back();
		//m_bufferPool.pop_back();

		//return bufferId;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::releaseBufferId( ALuint _sourceId )
	{
		alDeleteBuffers( 1, &_sourceId );

		//m_bufferPool.push_back( _sourceId );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::clearSourceId( ALuint _sourceId )
	{
		ALint process_count = 0;
		// Получаем количество отработанных буферов
		alGetSourcei( _sourceId, AL_BUFFERS_PROCESSED, &process_count );
		OAL_CHECK_ERROR();

		// Если таковые существуют то
		while( process_count-- > 0 )
		{
			// Исключаем их из очереди
			ALuint buffer = 0;

			alSourceUnqueueBuffers( _sourceId, 1, &buffer );
			OAL_CHECK_ERROR();
		}

		alSourceStop( _sourceId );

		ALint queued_count = 0;
		// unqueue remaining buffers
		alGetSourcei( _sourceId, AL_BUFFERS_QUEUED, &queued_count );
		OAL_CHECK_ERROR();

		while( queued_count-- > 0 )
		{
			// Исключаем их из очереди
			ALuint buffer = 0;

			alSourceUnqueueBuffers( _sourceId, 1, &buffer );
			OAL_CHECK_ERROR();
		}

		ALint state;
		do 
		{			
			alGetSourcei( _sourceId, AL_SOURCE_STATE, &state );
			OAL_CHECK_ERROR();
		}
		while( state == AL_PLAYING );


		alSourcei( _sourceId, AL_BUFFER, 0 );
	}
}	// namespace Menge
