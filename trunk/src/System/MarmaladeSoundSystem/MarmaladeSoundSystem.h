#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "MarmaladeSoundBufferBase.h"
#	include "MarmaladeSoundBufferStream.h"
#	include "MarmaladeSoundSource.h"

#   include "Utils/Factory/FactoryPool.h"

#	include <AL/al.h>
#	include <AL/alc.h>

namespace Menge
{
	class MarmaladeSoundSource;

	class ServiceProviderInterface;

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
		SoundSourceInterface* createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample ) override;

		SoundBufferInterface* createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream ) override;
		//SoundBufferInterface* createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem ) override;

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

        typedef FactoryPool<MarmaladeSoundBuffer, 32> TPoolOALSoundBuffer;
        TPoolOALSoundBuffer m_poolOALSoundBuffer;

        typedef FactoryPool<MarmaladeSoundBufferStream, 32> TPoolOALSoundBufferStream;
        TPoolOALSoundBufferStream m_poolOALSoundBufferStream;

        typedef FactoryPool<MarmaladeSoundSource, 32> TPoolOALSoundSource;
        TPoolOALSoundSource m_poolOALSoundSource;
    };
}	// namespace Menge
