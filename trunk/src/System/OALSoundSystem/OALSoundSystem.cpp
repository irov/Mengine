#	include "OALSoundSystem.h"

#	include "Interface/UnicodeInterface.h"
#	include "Interface/ThreadSystemInterface.h"

#	include "Logger/Logger.h"

#	include "OALError.h"

#	include <algorithm>
#	include <stdio.h>
#	include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Menge::SoundSystemInterface, Menge::OALSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundSystem::OALSoundSystem()
		: m_serviceProvider(nullptr)
		, m_context(nullptr)
		, m_device(nullptr)
		, m_threadAvaliable(false)
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

#	ifdef MENGINE_MARMALADE
		alcInit();
#	endif
		
        const ALCchar* defaultDeviceSprcifier = alcGetString( nullptr, ALC_DEVICE_SPECIFIER );
        
        LOGGER_WARNING(m_serviceProvider)("OpenAL default device specifier [%s]"
            , defaultDeviceSprcifier
            );
				
        m_device = alcOpenDevice( nullptr );
			
		if( m_device == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem.initialize: Failed to open 'generic software' sound device try default..." );

            m_device = alcOpenDevice( "Generic Software" );

            OAL_CHECK_ERROR(m_serviceProvider);

			if( m_device == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem.initialize: Failed to open default sound device try hardware" );

	    		m_device = alcOpenDevice( "Generic Hardware" );
                OAL_CHECK_ERROR(m_serviceProvider);

    			if( m_device == nullptr )
	    		{
		    		LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem.initialize: Failed to open hardware sound device.." );

				    return false;
			    }			
            }
		}

		const ALCchar* captureDeviceSpecifier = alcGetString( m_device, ALC_CAPTURE_DEVICE_SPECIFIER );

		LOGGER_WARNING(m_serviceProvider)("OpenAL capture device specifier [%s]"
			, captureDeviceSpecifier
			);		

		m_context = alcCreateContext( m_device, nullptr );

		if( m_context == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem: Failed to create context" );

			alcCloseDevice( m_device );
            OAL_CHECK_ERROR(m_serviceProvider);

			m_device = nullptr;

			return false;
		}

        ALCboolean currentResult = alcMakeContextCurrent( m_context );
        OAL_CHECK_ERROR(m_serviceProvider);

		if( currentResult == ALC_FALSE )
        {
            LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem: Failed to make context current" );

            alcDestroyContext( m_context );
            OAL_CHECK_ERROR(m_serviceProvider);
            m_context = nullptr;

            alcCloseDevice( m_device );
            OAL_CHECK_ERROR(m_serviceProvider);
            m_device = nullptr;

            return false;
        }
        		

		LOGGER_WARNING(m_serviceProvider)( "OpenAL driver properties" );

		LOGGER_WARNING(m_serviceProvider)( "Version: %s"
			, alGetString( AL_VERSION ) 
			);

		LOGGER_WARNING(m_serviceProvider)( "Vendor: %s"
			, alGetString( AL_VENDOR ) 
			);

		LOGGER_WARNING(m_serviceProvider)( "Renderer: %s"
			, alGetString( AL_RENDERER ) 
			);
	
		float lposition[] = { 0.0f, 0.0f, 0.0f };
		alListenerfv( AL_POSITION, lposition );
        OAL_CHECK_ERROR(m_serviceProvider);

        float lvelocity[] = { 0.0f, 0.0f, 0.0f };
		alListenerfv( AL_VELOCITY, lvelocity );
        OAL_CHECK_ERROR(m_serviceProvider);

        float lorient[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
		alListenerfv( AL_ORIENTATION, lorient );
        OAL_CHECK_ERROR(m_serviceProvider);

		m_threadAvaliable = THREAD_SYSTEM(m_serviceProvider)
			->avaliable();

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void OALSoundSystem::finalize()
    {
        if( m_device )
        {
            alcMakeContextCurrent( nullptr );
            
            if( m_context != nullptr )
            {
                alcDestroyContext( m_context );                
                m_context = nullptr;
            }

#   ifndef _DEBUG
            alcCloseDevice( m_device );
#   endif
            m_device = nullptr;
        }

#	ifdef MENGINE_MARMALADE
		alcDeinit();
#	endif
    }
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::update()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::onTurnSound( bool _turn )
	{
        (void)_turn;
        //if( _turn == false )
        //{
        //    alcMakeContextCurrent( NULL );
        //    //OAL_CHECK_ERROR(m_serviceProvider);

        //    alcSuspendContext( m_context );
        //    //OAL_CHECK_ERROR(m_serviceProvider);
        //}
        //else
        //{
        //    alcMakeContextCurrent( m_context );
        //    //OAL_CHECK_ERROR(m_serviceProvider);

        //    alcProcessContext( m_context );
        //    //OAL_CHECK_ERROR(m_serviceProvider);
        //}
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterfacePtr OALSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
	{
		//OALSoundSource* soundSource = m_soundSources.get();
		OALSoundSource * soundSource = m_poolOALSoundSource.createObjectT();

        soundSource->initialize( m_serviceProvider, this );
        		
		soundSource->setHeadMode( _isHeadMode );
		soundSource->setSoundBuffer( _buffer );

		return soundSource;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr OALSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream )
	{
		OALSoundBufferBase * base = nullptr;

        if( _isStream == false || m_threadAvaliable == false )
        {
            OALSoundBuffer * buffer = m_poolOALSoundBuffer.createObjectT();
            
            buffer->initialize( m_serviceProvider, this );

            base = buffer;
        }
        else
        {
            OALSoundBufferStream * buffer = m_poolOALSoundBufferStream.createObjectT();

            buffer->initialize( m_serviceProvider, this );

            base = buffer;
        }

		if( base->load( _soundDecoder ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem: Failed to create sound buffer from stream" 
				);

            return nullptr;
		}

		return base;
	}
	//////////////////////////////////////////////////////////////////////////
	ALuint OALSoundSystem::genSourceId()
	{
		ALuint sourceId = 0;
		alGenSources( 1, &sourceId );
        OAL_CHECK_ERROR(m_serviceProvider);

		return sourceId;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::releaseSourceId( ALuint _sourceId )
	{
		alDeleteSources( 1, &_sourceId );
        OAL_CHECK_ERROR(m_serviceProvider);
	}
	//////////////////////////////////////////////////////////////////////////
	ALuint OALSoundSystem::genBufferId()
	{
		ALuint bufferId = 0;
		alGenBuffers( 1, &bufferId );
        OAL_CHECK_ERROR(m_serviceProvider);

		return bufferId;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::releaseBufferId( ALuint _bufferId )
	{
		alDeleteBuffers( 1, &_bufferId );
        OAL_CHECK_ERROR(m_serviceProvider);
	}
}	// namespace Menge
