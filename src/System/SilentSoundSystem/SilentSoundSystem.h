#	pragma once

#	include "Interface/SoundSystemInterface.h"

#   include "Core/ServiceBase.h"

#   include "Factory/FactoryStore.h"

#	include "SilentSoundBuffer.h"
#	include "SilentSoundSource.h"

namespace Menge
{	
	class SilentSoundSystem
		: public ServiceBase<SoundSystemInterface>
	{
	public:
		SilentSoundSystem();
		~SilentSoundSystem();

	public:
		bool _initialize() override;
        void _finalize() override;

	public:
		void update() override;

	public:
		bool isSilent() const override;
		
	public:
		void onTurnSound( bool _turn ) override;

	public:
		SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample ) override;
		SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable ) override;

	public:
		uint32_t genSourceId();
		void releaseSourceId( uint32_t _sourceId );

		uint32_t genBufferId();
		void releaseBufferId( uint32_t _sourceId );

	public:
		void clearSourceId( uint32_t _sourceId );

	private:
        uint32_t m_enumerate;

        typedef FactoryPoolStore<SilentSoundBuffer, 32> TPoolSilentSoundBuffer;
        TPoolSilentSoundBuffer m_poolSilentSoundBuffer;

        typedef FactoryPoolStore<SilentSoundSource, 32> TPoolSilentSoundSource;
        TPoolSilentSoundSource m_poolSilentSoundSource;
	};
}	// namespace Menge
