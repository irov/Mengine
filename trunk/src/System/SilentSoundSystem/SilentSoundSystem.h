#	pragma once

#	include "Interface/SoundSystemInterface.h"

#   include "Factory/FactoryStore.h"

#	include "SilentSoundBuffer.h"
#	include "SilentSoundSource.h"

namespace Menge
{	
	class SilentSoundSystem
		: public SoundSystemInterface
	{
	public:
		SilentSoundSystem();
		~SilentSoundSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _provider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
        void finalize() override;
		
	public:
		void onTurnSound( bool _turn ) override;

	public:
		SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample ) override;
		SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable ) override;

	public:
		size_t genSourceId();
		void releaseSourceId( size_t _sourceId );

		size_t genBufferId();
		void releaseBufferId( size_t _sourceId );

	public:
		void clearSourceId( size_t _sourceId );

	private:
		ServiceProviderInterface * m_serviceProvider;

        size_t m_enumerate;

        typedef FactoryPoolStore<SilentSoundBuffer, 32> TPoolSilentSoundBuffer;
        TPoolSilentSoundBuffer m_poolSilentSoundBuffer;

        typedef FactoryPoolStore<SilentSoundSource, 32> TPoolSilentSoundSource;
        TPoolSilentSoundSource m_poolSilentSoundSource;
	};
}	// namespace Menge
