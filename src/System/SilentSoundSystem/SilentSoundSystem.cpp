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
		: m_serviceProvider(nullptr)
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
	void SilentSoundSystem::update()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::onTurnSound( bool _turn )
	{
        (void)_turn;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterfacePtr SilentSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
	{		
		SilentSoundSource * soundSource = m_poolSilentSoundSource.createObject();

		soundSource->initialize(this);
		
		soundSource->setHeadMode( _isHeadMode );
		soundSource->setSoundBuffer( _buffer );

		return soundSource;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr SilentSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable )
	{
        (void)_streamable;

		SilentSoundBuffer * buffer = m_poolSilentSoundBuffer.createObject();

		if( buffer->load( _soundDecoder ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("SilentSoundSystem::createSoundBuffer: failed to load sound buffer from decoder"
				);

            return nullptr;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t SilentSoundSystem::genSourceId()
	{
		uint32_t new_id = ++m_enumerate;

		return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::releaseSourceId( uint32_t _sourceId )
	{
        (void)_sourceId;

        //Empty
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t SilentSoundSystem::genBufferId()
	{
        uint32_t new_id = ++m_enumerate;

        return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::releaseBufferId( uint32_t _sourceId )
	{
        (void)_sourceId;

        //Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::clearSourceId( uint32_t _sourceId )
	{
        (void)_sourceId;

		//Empty
	}
}	// namespace Menge
