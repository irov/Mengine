#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "MarmaladeSoundBufferMemory.h"
#	include "MarmaladeSoundBufferStream.h"
#	include "MarmaladeSoundSource.h"

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

		uint32 carriage;
		uint32 size;
		int16 * memory;
		uint32 sampleStep;

		uint8 volume;
		int32 count;
		bool play;
		bool pause;
		bool stop;
		bool end;
	};
	//////////////////////////////////////////////////////////////////////////
#	ifndef MENGINE_MARMALADE_SOUND_MAX_COUNT
#	define MENGINE_MARMALADE_SOUND_MAX_COUNT 16
#	endif
	//////////////////////////////////////////////////////////////////////////
	class MarmaladeSoundSystem
		: public SoundSystemInterface
	{
	public:
		MarmaladeSoundSystem();
		~MarmaladeSoundSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		volatile MarmaladeSoundMemoryDesc * newSound();

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
		int32 mixSources_( int numSamples, int hasSamples, int16* target );

	protected:
		void setupFilter_();

	public:
		bool isDeviceStereo() const { return m_isDeviceStereo; }

	protected:
		ServiceProviderInterface * m_serviceProvider;

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
