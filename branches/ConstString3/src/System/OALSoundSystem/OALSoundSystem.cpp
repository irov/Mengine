#	include "OALSoundSystem.h"
#	include "Interface/LogSystemInterface.h"
#	include "Config/Config.h"

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
		: m_logService(NULL)
		, m_context(NULL)
		, m_device(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundSystem::~OALSoundSystem()
	{     

	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSystem::initialize( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;

		m_logService = m_serviceProvider->getServiceT<LogServiceInterface>("LogService");
		
		LOGGER_INFO(m_logService)( "Starting OpenAL Sound System..." );

		//const ALCchar *devices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);//

		//MENGE_LOG_ERROR( "OALSoundSystem.initialize: devices '%s'"
		//	, devices
		//	);
				
		// open default device
		
		m_device = alcOpenDevice( NULL );
			
		if( m_device == NULL )
		{
			LOGGER_ERROR(m_logService)( "OALSoundSystem.initialize: Failed to open 'generic software' sound device try default..." );

			const ALCchar* str = alcGetString( NULL, ALC_DEVICE_SPECIFIER );

            printf("ALC_DEVICE_SPECIFIER [%s]\n"
                , str                
                );

			// Открываем программное устройство
			//m_device = alcOpenDevice("Generic Software");
			m_device = alcOpenDevice( "Generic Software" );

			if( m_device == NULL )
			{
				LOGGER_ERROR(m_logService)( "OALSoundSystem.initialize: Failed to open default sound device try hardware" );

    			// open hardware device
	    		m_device = alcOpenDevice( "Generic Hardware" );	

    			if( m_device == NULL )
	    		{
		    		LOGGER_ERROR(m_logService)( "OALSoundSystem.initialize: Failed to open hardware sound device.." );

				    return false;
			    }			
            }
		}

		m_context = alcCreateContext( m_device, NULL );

		if( m_context == NULL )
		{
            LOGGER_ERROR(m_logService)( "OALSoundSystem: Failed to create context" );

			alcCloseDevice( m_device );
			m_device = NULL;

			return false;
		}

		if( alcMakeContextCurrent( m_context ) == ALC_FALSE )
        {
            LOGGER_ERROR(m_logService)( "OALSoundSystem: Failed to make context current" );

            alcDestroyContext( m_context );
            m_context = NULL;

            alcCloseDevice( m_device );
            m_device = NULL;

            return false;
        }
        		

		LOGGER_INFO(m_logService)( "OpenAL driver properties" );

		LOGGER_INFO(m_logService)( "Version: " );
		LOGGER_INFO(m_logService)( alGetString( AL_VERSION ) );

		LOGGER_INFO(m_logService)( "Vendor: " );
		LOGGER_INFO(m_logService)( alGetString( AL_VENDOR ) );

		LOGGER_INFO(m_logService)( "Renderer: " );
		LOGGER_INFO(m_logService)( alGetString( AL_RENDERER ) );

		if( alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE )
		{
			LOGGER_INFO(m_logService)( "Device Specifier: " );
			LOGGER_INFO(m_logService)( alcGetString( m_device, ALC_DEVICE_SPECIFIER ) );
		}
		//LOG( alGetString( AL_EXTENSIONS ) );

		float lposition[] = { 0.0f, 0.0f, 0.0f };
		float lvelocity[] = { 0.0f, 0.0f, 0.0f };
		float lorient[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
		alListenerfv( AL_POSITION, lposition );
		alListenerfv( AL_VELOCITY, lvelocity );
		alListenerfv( AL_ORIENTATION, lorient );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void OALSoundSystem::finalize()
    {
        alcMakeContextCurrent( NULL );

        if( m_context )
        {
            alcDestroyContext( m_context );
            m_context = NULL;
        }

        if( m_device )
        {
#   ifndef _DEBUG
            alcCloseDevice( m_device );
#   endif
            m_device = NULL;
        }
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
	LogServiceInterface * OALSoundSystem::getLogService() const
	{
		return m_logService;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::setListenerOrient( const float* _position, const float* _front, const float* _top )
	{
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

        if( _isStream == false )
        {
            buffer = new OALSoundBuffer( this );
        }
        else
        {
            buffer = new OALSoundBufferStream( this );
        }

		if( buffer->load( _soundDecoder ) == false )
		{
			LOGGER_ERROR(m_logService)( "OALSoundSystem: Failed to create sound buffer from stream" 
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
	ALuint OALSoundSystem::genSourceId()
	{
		ALuint sourceId = 0;
		alGenSources( 1, &sourceId );
        OAL_CHECK_ERROR();

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
        OAL_CHECK_ERROR();

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
        OAL_CHECK_ERROR();

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
        OAL_CHECK_ERROR();

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
        OAL_CHECK_ERROR();

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
        OAL_CHECK_ERROR();
	}
}	// namespace Menge
