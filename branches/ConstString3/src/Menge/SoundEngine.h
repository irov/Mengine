#	pragma once

#	include "Core/ConstString.h"
#	include "Core/Holder.h"
#	include	"math/vec3.h"

#	include "Interface/SoundSystemInterface.h"

#	include	<set>
//#	include	<string>
#	include <map>

namespace Menge
{
	class SoundEmitter;
	class ResourceSound;
	class SoundDecoder;
	class TaskSoundBufferUpdate;
	class FileInputStreamInterface;

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

	struct SoundSourceDesc
	{
		SoundSourceInterface* soundSourceInterface;
		SoundNodeListenerInterface* listener;
		TaskSoundBufferUpdate* taskSoundBufferUpdate;

		float timing;
		float volume;

		ESoundSourceState state;

		bool music;
		bool looped;
	};

	class SoundEngine
		: public Holder<SoundEngine>
	{
	public:
		SoundEngine();
		~SoundEngine();
	
    public:
		bool initialize();
        
    public:
        void onTurnSound( bool _turn );

    public:
		void setListenerOrient( const mt::vec3f& _position, const mt::vec3f& _front, const mt::vec3f& top );

		unsigned int createSoundSource(
			bool _isHeadMode, 
			SoundBufferInterface * _sample,
			bool _music = false );

		SoundBufferInterface * createSoundBufferFromFile( const ConstString& _pakName, const ConstString & _filename, const ConstString & _codecType, bool _isStream ); 
		SoundBufferInterface * createSoundBufferFromMemory( void* _buffer, int _size, bool _newmem );

		void setSoundVolume( float _volume );
		float getSoundVolume() const;

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
		void setLoop( unsigned int _emitter, bool _looped );
		bool getLoop( unsigned int _emitter );

		void setSourceVolume( unsigned int _emitter, float _volume );
		float getSourceVolume( unsigned int _emitter );

		void setSourceListener( unsigned int _emitter, SoundNodeListenerInterface* _listener );
		float getLengthMs( unsigned int _emitter ) const;
		
		void setPosMs( unsigned int _emitter, float _pos );
		float getPosMs( unsigned int _emitter ) const;

		void mute( bool _mute );
		bool isMute() const;

		void onFocus( bool _focus );

	protected:
		SoundSystemInterface * m_interface;
		SoundSulkCallback * m_sulkcallback;

	private:

		float m_soundVolume;
		float m_commonVolume;
		float m_musicVolume;

		struct SoundDesc
		{
			FileInputStreamInterface * stream;
			SoundDecoderInterface * codec;
		};

		typedef std::map<SoundBufferInterface*, SoundDesc> TMapBufferStreams;
		TMapBufferStreams m_bufferStreams;

		typedef std::map<unsigned int, SoundSourceDesc> TSoundSourceMap;
		TSoundSourceMap m_soundSourceMap;

		typedef std::vector<SoundNodeListenerInterface*> TSourceListenerVector;
		TSourceListenerVector m_stopListeners;
		TSourceListenerVector m_pauseListeners;

		bool m_initialized;
		bool m_muted;

		unsigned int m_enumerator;

		void updateVolume_();
	};
};
