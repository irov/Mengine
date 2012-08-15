
#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "Utils/Core/Pool.h"

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
	class LogServiceInterface;

	class OALSoundSystem
		: public SoundSystemInterface
	{
	public:
		OALSoundSystem();
		~OALSoundSystem();

	public:	// interfaces
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
        void finalize() override;
		
	public:
		void onTurnSound( bool _turn ) override;

	public:
		LogServiceInterface * getLogService() const;

	public:
		void setListenerOrient( const float* _position, const float* _front, const float* _top) override;
		SoundSourceInterface* createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample ) override;

		SoundBufferInterface* createSoundBuffer( SoundDecoderInterface* _soundDecoder, bool _isStream ) override;
		//SoundBufferInterface* createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem ) override;

		void releaseSoundBuffer( SoundBufferInterface * _soundBuffer ) override;
		void releaseSoundNode( SoundSourceInterface * _sn ) override;

	public:
		ALuint genSourceId();
		void releaseSourceId( ALuint _sourceId );

		ALuint genBufferId();
		void releaseBufferId( ALuint _sourceId );

	public:
		void clearSourceId( ALuint _sourceId );

	private:
		ServiceProviderInterface * m_serviceProvider;
		LogServiceInterface * m_logService;

		ALCcontext* m_context;
		ALCdevice* m_device;
	};
}	// namespace Menge
