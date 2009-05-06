#	pragma once

#	include "Holder.h"
#	include	"math/vec3.h"

#	include "Interface/SoundSystemInterface.h"

#	include	<set>
//#	include	<string>
#	include <map>

namespace Menge
{
	class SoundEmitter;
	class ResourceSound;
	class SoundDecoderInterface;
	class TaskSoundBufferUpdate;

	class SoundSulkCallback
		: public SoundSulkCallbackInterface
	{
	public:
		virtual ~SoundSulkCallback();
	};

	enum ESoundSourceState
	{
		Stopped = 0,	// currently stopped
		StopPlay,		// currently stopped, but need play
		Playing,		// currently playing
		Stopping,		// currently playing, but need stop
		Paused,			// currently paused
		Pausing,		// currently playing, but need pause
		PausePlay,		// currently paused, but need play
		NeedRestart		// currently playing, but need restart
	};
	struct TSoundSource
	{
		SoundSourceInterface* soundSourceInterface;
		ESoundSourceState state;
		SoundNodeListenerInterface* listener;
		float timing;
		bool looped;
		float volume;
		bool music;
		TaskSoundBufferUpdate* taskSoundBufferUpdate;

		TSoundSource( SoundSourceInterface* _interface, ESoundSourceState _state,
						SoundNodeListenerInterface* _listener, float _timing, bool _looped
						, float _volume, bool _music )
						: soundSourceInterface( _interface )
						, state( _state )
						, listener( _listener )
						, timing( _timing )
						, looped( _looped )
						, volume( _volume )
						, music( _music )
						, taskSoundBufferUpdate( NULL )
		{
		}
	};

	class SoundEngine
	{
	public:
		SoundEngine( SoundSystemInterface * _interface );
		~SoundEngine();
	
		bool initialize();

		void setListenerOrient( const mt::vec3f& _position, const mt::vec3f& _front, const mt::vec3f& top );

		unsigned int createSoundSource(
			bool _isHeadMode, 
			SoundBufferInterface * _sample,
			bool _music = false );

		SoundBufferInterface *	createSoundBufferFromFile( const String & _filename, bool _isStream ); 
		SoundBufferInterface *	createSoundBufferFromMemory( void* _buffer, int _size, bool _newmem );

		void setSoundSourceVolume( float _volume );
		float getSoundSourceVolume() const;

		void setCommonVolume( float _volume );
		float getCommonVolume() const;

		void setMusicVolume( float _volume );
		float getMusicVolume() const;

		void releaseSoundBuffer( SoundBufferInterface * _soundBuffer );
		void releaseSoundSource( unsigned int _sourceID );

		bool setBlow( bool _active );
		float getBlow();
		void setSulkCallback( SoundSulkCallback * _sulkcallback );

		void setEnoughBlow( float _enoughBlow );

		void update( float _timing );

		void play( unsigned int _emitter );
		void pause( unsigned int _emitter );
		void stop( unsigned int _emitter );
		void setLooped( unsigned int _emitter, bool _looped );
		bool isLooped( unsigned int _emitter );
		void setVolume( unsigned int _emitter, float _volume );
		float getVolume( unsigned int _emitter );
		void setSourceListener( unsigned int _emitter, SoundNodeListenerInterface* _listener );
		float getLengthMs( unsigned int _emitter );
		void setPosMs( unsigned int _emitter, float _pos );
		float getPosMs( unsigned int _emitter );

		void mute( bool _mute );
		void onFocus( bool _focus );

	protected:
		SoundSystemInterface * m_interface;
		SoundSulkCallback * m_sulkcallback;

	private:

		float m_soundVolume;
		float m_commonVolume;
		float m_musicVolume;

		typedef std::map< SoundBufferInterface*, SoundDecoderInterface* > TMapBufferStreams;
		TMapBufferStreams m_bufferStreams;

		typedef std::map< unsigned int, TSoundSource > TSoundSourceMap;
		TSoundSourceMap m_soundSourceMap;

		typedef std::vector< SoundNodeListenerInterface* > TSourceListenerVector;
		TSourceListenerVector m_stopListeners;
		TSourceListenerVector m_pauseListeners;

		bool m_initialized;
		bool m_muted;

		void updateVolume_();
	};
};
