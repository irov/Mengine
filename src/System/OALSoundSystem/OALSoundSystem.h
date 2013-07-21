#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "OALSoundBufferBase.h"
#	include "OALSoundBufferStream.h"
#	include "OALSoundSource.h"

#   include "Utils/Factory/FactoryPool.h"

#	include <al.h>
#	include <alc.h>

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

        typedef FactoryPool<OALSoundBuffer, 32> TPoolOALSoundBuffer;
        TPoolOALSoundBuffer m_poolOALSoundBuffer;

        typedef FactoryPool<OALSoundBufferStream, 32> TPoolOALSoundBufferStream;
        TPoolOALSoundBufferStream m_poolOALSoundBufferStream;

        typedef FactoryPool<OALSoundSource, 32> TPoolOALSoundSource;
        TPoolOALSoundSource m_poolOALSoundSource;
    };
}	// namespace Menge
