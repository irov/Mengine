#	pragma once

#	include "Interface/SoundSystemInterface.h"

#ifdef __APPLE__
#   include <OpenAL/al.h>
#   include <OpenAL/alc.h>
#else
#	include <AL/al.h>
#	include <AL/alc.h>
#endif

namespace Menge
{
	class OALSoundSource;

	class ServiceProviderInterface;

	class OALSoundSystem
		: public SoundSystemInterface
	{
	public:
		OALSoundSystem();
		~OALSoundSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
        void finalize() override;
		
	public:
		void onTurnSound( bool _turn ) override;

	public:
		SoundSourceInterface* createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample ) override;

		SoundBufferInterface* createSoundBuffer( SoundDecoderInterface* _soundDecoder, bool _isStream ) override;
		//SoundBufferInterface* createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem ) override;

		void releaseSoundBuffer( SoundBufferInterface * _soundBuffer ) override;
		void releaseSoundNode( SoundSourceInterface * _sn ) override;

	public:
		ALuint genSourceId();
		void releaseSourceId( ALuint _sourceId );

		ALuint genBufferId();
		void releaseBufferId( ALuint _bufferId );

	public:
		void clearSourceId( ALuint _sourceId );

	private:
		ServiceProviderInterface * m_serviceProvider;

		ALCcontext* m_context;
		ALCdevice* m_device;
	};
}	// namespace Menge
