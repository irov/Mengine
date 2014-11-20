#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "MarmaladeSoundBufferMemory.h"
#	include "MarmaladeSoundBufferStream.h"
#	include "MarmaladeSoundSource.h"

#   include "Factory/FactoryStore.h"

#	include <s3eSound.h>

namespace Menge
{
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
		bool initialize() override;
        void finalize() override;
		
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
    };
}	// namespace Menge
