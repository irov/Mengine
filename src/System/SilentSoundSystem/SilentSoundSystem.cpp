#	include "SilentSoundSystem.h"

#	include "Config/Config.h"
#	include "Logger/Logger.h"

#   include "Factory/FactoryPool.h"

#	include <algorithm>
#	include <stdio.h>
#	include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SilentSoundSystem, Menge::SilentSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SilentSoundSystem::SilentSoundSystem()
		: m_enumerate(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SilentSoundSystem::~SilentSoundSystem()
	{     

	}
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundSystem::_initialize()
	{
        LOGGER_INFO(m_serviceProvider)( "Starting Silent Sound System..." );
        		
		m_factorySilentSoundBuffer = new FactoryPool<SilentSoundBuffer, 32>( m_serviceProvider );
		m_factorySilentSoundSource = new FactoryPool<SilentSoundSource, 32>( m_serviceProvider );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::_finalize()
    {
        
    }
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::update()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool SilentSoundSystem::isSilent() const
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::onTurnSound( bool _turn )
	{
        (void)_turn;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterfacePtr SilentSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
	{		
		SilentSoundSource * soundSource = m_factorySilentSoundSource->createObject();

		soundSource->initialize(this);
		
		soundSource->setHeadMode( _isHeadMode );
		soundSource->setSoundBuffer( _buffer );

		return soundSource;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr SilentSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable )
	{
        (void)_streamable;

		SilentSoundBuffer * buffer = m_factorySilentSoundBuffer->createObject();

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
