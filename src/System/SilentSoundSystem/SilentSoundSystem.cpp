#	include "SilentSoundSystem.h"

#	include "Interface/LogSystemInterface.h"

#	include "Config/Config.h"
#	include "Logger/Logger.h"

#	include <algorithm>
#	include <stdio.h>
#	include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SilentSoundSystem, Menge::SoundSystemInterface, Menge::SilentSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SilentSoundSystem::SilentSoundSystem()
		: m_serviceProvider(NULL)
        , m_enumerate(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SilentSoundSystem::~SilentSoundSystem()
	{     

	}
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * SilentSoundSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundSystem::initialize()
	{
        LOGGER_INFO(m_serviceProvider)( "Starting Silent Sound System..." );
        		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::finalize()
    {
        
    }
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::onTurnSound( bool _turn )
	{
        (void)_turn;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterface* SilentSoundSystem::createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample )
	{		
		SilentSoundSource * soundSource = m_poolSilentSoundSource.createObjectT();

		soundSource->initialize(this);
		
		soundSource->setHeadMode( _isHeadMode );
		soundSource->setSoundBuffer( _sample );

		return soundSource;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface* SilentSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream )
	{
        (void)_isStream;

		SilentSoundBuffer * buffer = m_poolSilentSoundBuffer.createObjectT();

		if( buffer->load( _soundDecoder ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "SilentSoundSystem: failed to load sound buffer from decoder" 
				);

			buffer->destroy();
			
            return nullptr;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t SilentSoundSystem::genSourceId()
	{
		size_t new_id = ++m_enumerate;

		return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::releaseSourceId( size_t _sourceId )
	{
        (void)_sourceId;

        //Empty
	}
	//////////////////////////////////////////////////////////////////////////
	size_t SilentSoundSystem::genBufferId()
	{
        size_t new_id = ++m_enumerate;

        return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::releaseBufferId( size_t _sourceId )
	{
        (void)_sourceId;

        //Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::clearSourceId( size_t _sourceId )
	{
        (void)_sourceId;

		//Empty
	}
}	// namespace Menge
