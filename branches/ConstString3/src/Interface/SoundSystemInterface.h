#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class SoundSourceInterface;
	class SoundDecoderInterface;
	class LogSystemInterface;

	class SoundNodeListenerInterface
	{
	public:
		virtual void listenSoundNodePaused() = 0;
		virtual void listenSoundNodeStopped() = 0;
	};

	class SoundBufferInterface
	{
	public:
		virtual void release() = 0;
		virtual void update() = 0;
	};

	class SoundSourceInterface
	{
	public:
		virtual void play() = 0;
		virtual void pause() = 0;
		virtual void stop() = 0;

		virtual bool isPlaying() const = 0;

		virtual void setVolume( float _volume ) = 0;
		virtual float getVolume() const = 0;

		virtual void setPosition( float _x, float _y, float _z ) = 0;
		virtual const float * getPosition() const = 0;

		virtual void setLoop( bool _loop ) = 0;
		virtual bool getLoop() const = 0;

		virtual float getLengthMs() = 0;
		virtual float getPosMs() = 0;
		virtual void setPosMs( float _posMs ) = 0;

		virtual void loadBuffer( SoundBufferInterface* _soundBuffer ) = 0;
		virtual SoundBufferInterface* getSoundBuffer() = 0;
	};

	class SoundSulkCallbackInterface
	{
	public:
		virtual void blow( float _blow ) = 0;
	};

	class SoundSystemInterface
	{
	public:
		virtual bool initialize( LogSystemInterface* _logSystem ) = 0;

		virtual void onTurnSound( bool _turn ) = 0;

		virtual void setListenerOrient( const float * _position, const float * _front, const float * top) = 0;
		virtual SoundSourceInterface * createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample ) = 0;

		virtual SoundBufferInterface* createSoundBuffer( SoundDecoderInterface* _soundDecoder, bool _isStream ) = 0;
		//virtual SoundBufferInterface * createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem ) = 0;

		virtual void releaseSoundBuffer( SoundBufferInterface * _soundBuffer ) = 0;
		virtual void releaseSoundNode( SoundSourceInterface * _sn ) = 0;

		virtual bool setBlow( bool _active ) = 0;
		virtual float getBlow() = 0;

		virtual void setEnoughBlow( float _enoughBlow ) = 0;
		virtual void setBlowCallback( SoundSulkCallbackInterface * _callback ) = 0;

		virtual void update( float _timing ) = 0;
	};
}

bool initInterfaceSystem( Menge::SoundSystemInterface** );
void releaseInterfaceSystem( Menge::SoundSystemInterface* );
