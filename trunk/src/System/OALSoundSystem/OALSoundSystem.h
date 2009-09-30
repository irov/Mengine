
#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include <AL/al.h>
#	include <Al/alc.h>

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
		ALuint popSource( bool _isStereo );
		void pushSource( ALuint _source, bool _isStereo );

	public:
		void log( const char* _message, ... );
		void log_error( const char* _message, ... );

	private:
		LogSystemInterface * m_logSystem;

		bool m_initialized;
		typedef std::vector<ALuint> TSourcePool;
		TSourcePool m_monoPool;
		TSourcePool m_stereoPool;

		ALCcontext* m_context;
		ALCdevice* m_device;

		SulkSystem * m_sulk;

		typedef std::vector<OALSoundSource*> TSoundSourceVector;
		TSoundSourceVector m_soundSourcePool;
	};
}	// namespace Menge
