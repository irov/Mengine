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

	protected:
		float carriageToPosition_( uint32_t _carriage, uint32_t _frequency ) const;
		uint32_t positionToCarriage_( float _position, uint32_t _frequency ) const;

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
		void formActiveChannelsAndDecode_( uint32_t & _samplesAvailable );
		int32_t mixSources_( int numSamples, int hasSamples, int16_t * target );

	protected:
		ServiceProviderInterface * m_serviceProvider;

        typedef FactoryPoolStore<SDLSoundBufferMemory, 32> TPoolOALSoundBuffer;
        TPoolOALSoundBuffer m_poolOALSoundBuffer;

		typedef FactoryPoolStore<SDLSoundBufferStream, 32> TPoolOALSoundBufferStream;
        TPoolOALSoundBufferStream m_poolOALSoundBufferStream;

		typedef FactoryPoolStore<SDLSoundSource, 32> TPoolOALSoundSource;
        TPoolOALSoundSource m_poolOALSoundSource;
    };
}	// namespace Menge
