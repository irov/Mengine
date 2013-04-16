#	include "SilentSoundSystem.h"

#	include "SilentSoundBuffer.h"
#	include "SilentSoundSource.h"

#	include "Interface/LogSystemInterface.h"

#	include "Config/Config.h"
#	include "Logger/Logger.h"

#	include <algorithm>
#	include <stdio.h>
#	include <stdarg.h>

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool createSilentSoundSystem( Menge::SoundSystemInterface ** _soundSystem )
    {
        if( _soundSystem == 0 )
        {
            return false;
        }

        *_soundSystem = new Menge::SilentSoundSystem();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void destroySilentSoundSystem( Menge::SoundSystemInterface * _soundSystem )
    {
        if( _soundSystem )
        {
            delete static_cast<Menge::SilentSoundSystem *>(_soundSystem);
        }
    }
}
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
        delete this;
    }
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::onTurnSound( bool _turn )
	{
        (void)_turn;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterface* SilentSoundSystem::createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample )
	{
		//OALSoundSource* soundSource = m_soundSources.get();
		SilentSoundSource * soundSource = new SilentSoundSource;

		soundSource->initialize(this);
		
		soundSource->setHeadMode( _isHeadMode );
		soundSource->loadBuffer( _sample );

		return soundSource;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface* SilentSoundSystem::createSoundBuffer( SoundDecoderInterface* _soundDecoder, bool _isStream )
	{
        (void)_isStream;

		SilentSoundBuffer * buffer = new SilentSoundBuffer();

		if( buffer->load( _soundDecoder ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "SilentSoundSystem: failed to load sound buffer from decoder" 
				);

			buffer->release();
			
            return NULL;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::releaseSoundBuffer( SoundBufferInterface * _soundBuffer )
	{
		if( _soundBuffer != NULL )
		{
			_soundBuffer->release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SilentSoundSystem::releaseSoundNode( SoundSourceInterface * _sn )
	{
		SilentSoundSource * soundSource = static_cast<SilentSoundSource *>( _sn );

		delete soundSource;
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
