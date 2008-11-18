#	pragma once

#	include "../Config/Typedef.h"

namespace Menge
{
	class SoundSourceInterface;
	class DataStreamInterface;
	class LogSystemInterface;

	class	SoundNodeListenerInterface
	{
	public:
		virtual void listenPaused( bool _pause ) = 0;
		virtual void listenStopped() = 0;
	};

	class SoundBufferInterface
	{
	public:
		virtual ~SoundBufferInterface() {}
	};

	class SoundSourceInterface
	{
	public:
		//virtual ~SoundSourceInterface(){};

	public:
		virtual void play() = 0;
		virtual void pause() = 0;
		virtual void stop() = 0;
		virtual bool isPlaying() const = 0;

	public:	
		virtual void setVolume( float _volume ) = 0;
		virtual float getVolume() const = 0;

	public:
		virtual void setPosition( float _x, float _y, float _z ) = 0;
		virtual const float * getPosition() const = 0;

	public:
		virtual void setLooped( bool _loop ) = 0;
		virtual bool isLooping() const = 0;

	public:
		virtual int	getLengthMs() = 0;
		virtual int getPosMs() = 0;
		virtual void setPosMs( float _posMs ) = 0;

		virtual void setSoundNodeListener(SoundNodeListenerInterface * _listener) = 0;

		virtual void setSoundBuffer( SoundBufferInterface* _soundBuffer ) = 0;
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

		virtual void setListenerOrient( float * _position, float * _front, float * top) = 0;
		virtual SoundSourceInterface * createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample, SoundNodeListenerInterface * _listener ) = 0;

		virtual SoundBufferInterface* createSoundBuffer( DataStreamInterface* _stream, bool _isStream ) = 0;
		virtual SoundBufferInterface * createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem ) = 0;

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