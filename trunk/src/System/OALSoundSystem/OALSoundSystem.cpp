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

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Menge::SoundSystemInterface, Menge::OALSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
#	define MAX_SOUND_SOURCES 32
#	define MAX_SOUND_BUFFERS 16

	//////////////////////////////////////////////////////////////////////////
	OALSoundSystem::OALSoundSystem()
		: m_serviceProvider(nullptr)
		, m_context(nullptr)
		, m_device(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundSystem::~OALSoundSystem()
	{     

	}
    //////////////////////////////////////////////////////////////////////////
    void OALSoundSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * OALSoundSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSystem::initialize()
	{
		LOGGER_INFO(m_serviceProvider)( "Starting OpenAL Sound System..." );

		//const ALCchar *devices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);//

		//MENGE_LOG_ERROR( "OALSoundSystem.initialize: devices '%s'"
		//	, devices
		//	);
				
		// open default device
		
		m_device = alcOpenDevice( NULL );
			
		if( m_device == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem.initialize: Failed to open 'generic software' sound device try default..." );

			const ALCchar* str = alcGetString( NULL, ALC_DEVICE_SPECIFIER );

            printf("ALC_DEVICE_SPECIFIER [%s]\n"
                , str                
                );

			// ��������� ����������� ����������
			//m_device = alcOpenDevice("Generic Software");
			m_device = alcOpenDevice( "Generic Software" );

			if( m_device == NULL )
			{
				LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem.initialize: Failed to open default sound device try hardware" );

    			// open hardware device
	    		m_device = alcOpenDevice( "Generic Hardware" );	

    			if( m_device == NULL )
	    		{
		    		LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem.initialize: Failed to open hardware sound device.." );

				    return false;
			    }			
            }
		}

		m_context = alcCreateContext( m_device, NULL );

		if( m_context == NULL )
		{
            LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem: Failed to create context" );

			alcCloseDevice( m_device );
			m_device = NULL;

			return false;
		}

		if( alcMakeContextCurrent( m_context ) == ALC_FALSE )
        {
            LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem: Failed to make context current" );

            alcDestroyContext( m_context );
            m_context = NULL;

            alcCloseDevice( m_device );
            m_device = NULL;

            return false;
        }
        		

		LOGGER_INFO(m_serviceProvider)( "OpenAL driver properties" );

		LOGGER_INFO(m_serviceProvider)( "Version: " );
		LOGGER_INFO(m_serviceProvider)( alGetString( AL_VERSION ) );

		LOGGER_INFO(m_serviceProvider)( "Vendor: " );
		LOGGER_INFO(m_serviceProvider)( alGetString( AL_VENDOR ) );

		LOGGER_INFO(m_serviceProvider)( "Renderer: " );
		LOGGER_INFO(m_serviceProvider)( alGetString( AL_RENDERER ) );

		if( alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE )
		{
			LOGGER_INFO(m_serviceProvider)( "Device Specifier: " );
			LOGGER_INFO(m_serviceProvider)( alcGetString( m_device, ALC_DEVICE_SPECIFIER ) );
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
        if( m_device )
        {
            alcMakeContextCurrent( NULL );

            if( m_context )
            {
                alcDestroyContext( m_context );
                m_context = NULL;
            }

#   ifndef _DEBUG
            alcCloseDevice( m_device );
#   endif
            m_device = NULL;
        }
        
        delete this;
    }
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::onTurnSound( bool _turn )
	{
        (void)_turn;
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
	SoundSourceInterface* OALSoundSystem::createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample )
	{
		//OALSoundSource* soundSource = m_soundSources.get();
		OALSoundSource* soundSource = new OALSoundSource( m_serviceProvider, this );
        		
		soundSource->setHeadMode( _isHeadMode );
		soundSource->loadBuffer( _sample );

		return soundSource;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface* OALSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream )
	{
		OALSoundBufferBase * buffer = NULL;

        if( _isStream == false )
        {
            buffer = new OALSoundBuffer( m_serviceProvider, this );
        }
        else
        {
            buffer = new OALSoundBufferStream( m_serviceProvider, this );
        }

		if( buffer->load( _soundDecoder ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem: Failed to create sound buffer from stream" 
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
        OAL_CHECK_ERROR(m_serviceProvider);

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
        OAL_CHECK_ERROR(m_serviceProvider);

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
        OAL_CHECK_ERROR(m_serviceProvider);

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
	void OALSoundSystem::releaseBufferId( ALuint _bufferId )
	{
		alDeleteBuffers( 1, &_bufferId );
        OAL_CHECK_ERROR(m_serviceProvider);

		//m_bufferPool.push_back( _sourceId );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::clearSourceId( ALuint _sourceId )
	{
		ALint process_count = 0;
		// �������� ���������� ������������ �������
		alGetSourcei( _sourceId, AL_BUFFERS_PROCESSED, &process_count );
		OAL_CHECK_ERROR(m_serviceProvider);

		// ���� ������� ���������� ��
		while( process_count-- > 0 )
		{
			// ��������� �� �� �������
			ALuint buffer = 0;

			alSourceUnqueueBuffers( _sourceId, 1, &buffer );
			OAL_CHECK_ERROR(m_serviceProvider);
		}

		alSourceStop( _sourceId );
        OAL_CHECK_ERROR(m_serviceProvider);

		ALint queued_count = 0;
		// unqueue remaining buffers
		alGetSourcei( _sourceId, AL_BUFFERS_QUEUED, &queued_count );
		OAL_CHECK_ERROR(m_serviceProvider);

		while( queued_count-- > 0 )
		{
			// ��������� �� �� �������
			ALuint buffer = 0;

			alSourceUnqueueBuffers( _sourceId, 1, &buffer );
			OAL_CHECK_ERROR(m_serviceProvider);
		}

		ALint state;
		do 
		{			
			alGetSourcei( _sourceId, AL_SOURCE_STATE, &state );
			OAL_CHECK_ERROR(m_serviceProvider);
		}
		while( state == AL_PLAYING );
        
		alSourcei( _sourceId, AL_BUFFER, 0 );
        OAL_CHECK_ERROR(m_serviceProvider);
	}
}	// namespace Menge
