#	include "SDLSoundSystem.h"

#	include "SDLSoundConfig.h"
#	include "SDLSoundError.h"
#	include "SDLSoundSource.h"

#	include "Logger/Logger.h"

#	include <math.h>

#	include "SDL.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Menge::SoundSystemInterface, Menge::SDLSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SDLSoundSystem::SDLSoundSystem()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SDLSoundSystem::~SDLSoundSystem()
	{     

	}
    //////////////////////////////////////////////////////////////////////////
	void SDLSoundSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * SDLSoundSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////	
	uint32_t SDLSoundSystem::playSoundDesc( SDLSoundSource * _source, float _position, float _volume, int32_t _count )
	{
		return INVALID_SOUND_ID;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLSoundSystem::removeSoundDesc( uint32_t _id )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLSoundSystem::stopSoundDesc( uint32_t _id )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLSoundSystem::pauseSoundDesc( uint32_t _id, float & _position )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLSoundSystem::resumeSoundDesc( uint32_t _id, float _position )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLSoundSystem::setSoundDescVolume( uint32_t _id, float _volume )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLSoundSystem::getSoundDescPosition( uint32_t _id, float & _position )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLSoundSystem::initialize()
	{				
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLSoundSystem::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLSoundSystem::update()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLSoundSystem::onTurnSound( bool _turn )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr SDLSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream )
	{
		_isStream = false;

		SDLSoundBufferPtr base;

		if( _isStream == false )
		{
			SDLSoundBufferMemory * buffer = m_poolSoundBuffer.createObjectT();

			buffer->setServiceProvider( m_serviceProvider );

			base = buffer;
		}
		else
		{
			SDLSoundBufferStream * buffer = m_poolSoundBufferStream.createObjectT();

			buffer->setServiceProvider( m_serviceProvider );

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
	SoundSourceInterfacePtr SDLSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
	{
		SDLSoundSource * soundSource = m_poolSoundSource.createObjectT();

        soundSource->initialize( m_serviceProvider, this );
        		
		soundSource->setSoundBuffer( _buffer );
		
		return soundSource;
	}
}	// namespace Menge

