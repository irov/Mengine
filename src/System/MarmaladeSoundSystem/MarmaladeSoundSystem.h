#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "MarmaladeSoundBufferMemory.h"
#	include "MarmaladeSoundBufferStream.h"
#	include "MarmaladeSoundSource.h"

#	include "MarmaladeSoundFilter.h"

#   include "Factory/FactoryStore.h"

#	include <s3eSound.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class MarmaladeSoundSource;
	//////////////////////////////////////////////////////////////////////////
	struct MarmaladeSoundMemoryDesc
	{
		MarmaladeSoundSource * source;

		uint32_t carriage;
		uint32_t size;
		int16 * memory;
		uint32_t frequency;
		uint32_t channels;

		uint32_t W;
		uint32_t L;

		uint8 volume;
		int32 count;
		bool play;
		bool pause;
		bool stop;
		bool end;

	public:
		inline int32 getSample( uint32_t _channel ) volatile const
		{
			uint32_t source_carriage = this->carriage * this->W / this->L;
			
			uint32_t position = source_carriage * this->channels;

			int32 pure_value = (int32)this->memory[ position + _channel ];

			int32 volume_value = (pure_value * this->volume) >> 8;

			return volume_value;
		}

		inline bool isDone() volatile const
		{
			uint32_t position = this->carriage * this->channels;

			return position >= this->size;
		}

		inline void setCarriage( float _position ) volatile
		{
			uint32_t source_carriage = uint32_t(_position) * this->frequency * this->channels / 1000;

			this->carriage = source_carriage * this->L / this->W;
		}

		inline void setSize( float _length ) volatile
		{
			uint32_t source_size = uint32_t(_length) * this->frequency * this->channels / 1000;

			this->size = source_size * this->L / this->W;
		}

		inline void setVolume( float _volume ) volatile
		{
			uint8 volume_s3e = (uint8)(_volume * (S3E_SOUND_MAX_VOLUME - 1));

			this->volume = volume_s3e;
		}
	};
	//////////////////////////////////////////////////////////////////////////
#	ifndef MENGINE_MARMALADE_SOUND_MAX_COUNT
#	define MENGINE_MARMALADE_SOUND_MAX_COUNT 16
#	endif
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t INVALID_SOUND_ID = (uint32_t)-1;
	//////////////////////////////////////////////////////////////////////////
	class MarmaladeSoundSystem
		: public ServiceBase<SoundSystemInterface>
	{
	public:
		MarmaladeSoundSystem();
		~MarmaladeSoundSystem();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		uint32_t playSoundDesc( MarmaladeSoundSource * _source, float _position, float _volume, int32 _count );
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
		void update() override;

	public:
		bool isSilent() const override;
		
	public:
		void onTurnSound( bool _turn ) override;

	public:		
		SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream ) override;

	public:
		SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer ) override;

	protected:
		void formActiveChannelsAndDecode_( uint32_t & _samplesAvailable );
		int32 mixSources_( int numSamples, int hasSamples, int16* target );

	protected:
		void setupFilter_();

	public:
		bool isDeviceStereo() const { return m_isDeviceStereo; }

	protected:
        typedef FactoryPoolStore<MarmaladeSoundBufferMemory, 32> TPoolOALSoundBuffer;
        TPoolOALSoundBuffer m_poolOALSoundBuffer;

        typedef FactoryPoolStore<MarmaladeSoundBufferStream, 32> TPoolOALSoundBufferStream;
        TPoolOALSoundBufferStream m_poolOALSoundBufferStream;

        typedef FactoryPoolStore<MarmaladeSoundSource, 32> TPoolOALSoundSource;
        TPoolOALSoundSource m_poolOALSoundSource;

		bool m_isDeviceStereo;
		int32 m_soundOutputFrequence;

		volatile MarmaladeSoundMemoryDesc m_soundMemoryDesc[MENGINE_MARMALADE_SOUND_MAX_COUNT];

		int m_soundChannel;
    };
}	// namespace Menge
