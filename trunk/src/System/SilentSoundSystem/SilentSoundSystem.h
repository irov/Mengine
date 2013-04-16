
#	pragma once

#	include "Interface/SoundSystemInterface.h"

namespace Menge
{
	class SilentSoundSource;

	class ServiceProviderInterface;
	class LogServiceInterface;

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
		SoundSourceInterface* createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample ) override;

		SoundBufferInterface* createSoundBuffer( SoundDecoderInterface* _soundDecoder, bool _isStream ) override;

		void releaseSoundBuffer( SoundBufferInterface * _soundBuffer ) override;
		void releaseSoundNode( SoundSourceInterface * _sn ) override;

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
	};
}	// namespace Menge
