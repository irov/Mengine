#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "SDLSoundBufferMemory.h"
#	include "SDLSoundBufferStream.h"
#	include "SDLSoundSource.h"

#	include "SDLSoundFilter.h"

#   include "Factory/FactoryStore.h"

#	include "SDL.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t INVALID_SOUND_ID = (uint32_t)-1;
	//////////////////////////////////////////////////////////////////////////
	class SDLSoundSystem
		: public SoundSystemInterface
	{
	public:
		SDLSoundSystem();
		~SDLSoundSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		uint32_t playSoundDesc( SDLSoundSource * _source, float _position, float _volume, int32_t _count );
		void removeSoundDesc( uint32_t _id );
		bool stopSoundDesc( uint32_t _id );

		bool pauseSoundDesc( uint32_t _id, float & _position );
		bool resumeSoundDesc( uint32_t _id, float _position );

		bool setSoundDescVolume( uint32_t _id, float _volume );

		bool getSoundDescPosition( uint32_t _id, float & _position );

	public:
		bool initialize() override;
        void finalize() override;

		void update() override;
		
	public:
		void onTurnSound( bool _turn ) override;

	public:		
		SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream ) override;

	public:
		SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

        typedef FactoryPoolStore<SDLSoundBufferMemory, 32> TPoolSoundBuffer;
        TPoolSoundBuffer m_poolSoundBuffer;

		typedef FactoryPoolStore<SDLSoundBufferStream, 32> TPoolSoundBufferStream;
        TPoolSoundBufferStream m_poolSoundBufferStream;

		typedef FactoryPoolStore<SDLSoundSource, 32> TPoolSoundSource;
        TPoolSoundSource m_poolSoundSource;
    };
}	// namespace Menge
