
#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "Utils/Core/Pool.h"

#ifdef __APPLE__
#   include <OpenAL/al.h>
#   include <OpenAL/alc.h>
#else
#	include <al.h>
#	include <alc.h>
#endif

class SulkSystem;

namespace Menge
{
	class OALSoundSource;

	class OALSoundSystem
		: public SoundSystemInterface
	{
	public:
		OALSoundSystem();
		~OALSoundSystem();

	public:	// interfaces
		bool initialize( LogSystemInterface* _logSystem ) override;
		
	public:
		void onTurnSound( bool _turn ) override;

	public:
		LogSystemInterface * getLogSystem() const;

	public:
		void setListenerOrient( const float* _position, const float* _front, const float* _top) override;
		SoundSourceInterface* createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample ) override;

		SoundBufferInterface* createSoundBuffer( SoundDecoderInterface* _soundDecoder, bool _isStream ) override;
		//SoundBufferInterface* createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem ) override;

		void releaseSoundBuffer( SoundBufferInterface * _soundBuffer ) override;
		void releaseSoundNode( SoundSourceInterface * _sn ) override;

		bool setBlow( bool _active ) override;
		float getBlow() override;

		void setEnoughBlow( float _enoughBlow ) override;
		void setBlowCallback( SoundSulkCallbackInterface * _callback ) override;

		void update( float _timing ) override;

	public:
		ALuint genSourceId();
		void releaseSourceId( ALuint _sourceId );

		ALuint genBufferId();
		void releaseBufferId( ALuint _sourceId );

	public:
		void clearSourceId( ALuint _sourceId );

	private:
		LogSystemInterface * m_logSystem;

		bool m_initialized;

		typedef std::vector<ALuint> TVectorSourceId;
		TVectorSourceId m_sourcePool;

		typedef std::vector<ALuint> TVectorBufferId;
		TVectorBufferId m_bufferPool;

		ALCcontext* m_context;
		ALCdevice* m_device;

		SulkSystem * m_sulk;

		//typedef Pool<OALSoundSource> TPoolSoundSource;
		//TPoolSoundSource m_soundSources;
	};
}	// namespace Menge
