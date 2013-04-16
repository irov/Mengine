#	pragma once

#	include "Core/ConstString.h"
#	include	"math/vec3.h"

#	include "Interface/SoundSystemInterface.h"

#	include <vector>
#	include <map>

namespace Menge
{
    class ThreadTaskSoundBufferUpdate;
	class InputStreamInterface;

	enum ESoundSourceState
	{
		ESS_STROPPED = 0,	    // currently stopped
		ESS_STOP_PLAY = 1,		// currently stopped, but need play
		ESS_PLAYING = 2,		// currently playing
		ESS_STOPPING = 3,		// currently playing, but need stop
		ESS_PAUSED = 4,			// currently paused
		ESS_PAUSING = 5,		// currently playing, but need pause
		ESS_PAUSE_PLAY = 6,		// currently paused, but need play
		ESS_NEED_RESTART = 7	// currently playing, but need restart
	};

	struct SoundSourceDesc
	{
		SoundSourceInterface* soundSourceInterface;
		SoundNodeListenerInterface* listener;
		ThreadTaskSoundBufferUpdate* taskSoundBufferUpdate;

		float timing;
		float volume;

		ESoundSourceState state;
		ESoundSourceType type;

        bool streamable;
		bool looped;
	};

	class SoundEngine
		: public SoundServiceInterface
	{
	public:
		SoundEngine();
		~SoundEngine();
	
    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
		bool initialize( bool _silent ) override;
		void finalize() override;

	public:
		void update( float _timing ) override;
        
    public:
		void onTurnStream( bool _turn ) override;
        void onTurnSound( bool _turn ) override;

    public:
		unsigned int createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample, ESoundSourceType _type, bool _streamable ) override;

    public:
		SoundBufferInterface * createSoundBufferFromFile( const ConstString& _pakName, const FilePath & _filename, const ConstString & _codecType, bool _isStream ) override;  

    public:
		void setSoundVolume( float _volume ) override;
		float getSoundVolume() const override;

		void setCommonVolume( float _volume ) override;
		float getCommonVolume() const override;

		void setMusicVolume( float _volume ) override;
		float getMusicVolume() const override;

        void setVoiceVolume( float _volume ) override;
        float getVoiceVolume() const override;

		bool setSourceVolume( unsigned int _emitter, float _volume ) override;
		float getSourceVolume( unsigned int _emitter ) const override;

		void releaseSoundBuffer( SoundBufferInterface * _soundBuffer ) override;
		void releaseSoundSource( unsigned int _sourceID ) override;

		bool validSoundSource( unsigned int _sourceID ) const override;
		
	public:
		bool play( unsigned int _emitter ) override;
		bool pause( unsigned int _emitter ) override;
		bool stop( unsigned int _emitter ) override;

	public:
		void setLoop( unsigned int _emitter, bool _looped ) override;
		bool getLoop( unsigned int _emitter ) const override;

	public:
		void setSourceListener( unsigned int _emitter, SoundNodeListenerInterface* _listener ) override;
		
	public:
		float getLengthMs( unsigned int _emitter ) const override;
		
		void setPosMs( unsigned int _emitter, float _pos ) override;
		float getPosMs( unsigned int _emitter ) const override;

		void mute( bool _mute ) override;
		bool isMute() const override;

        bool isSilent() const override;

    protected:
        void updateSourceVolume_( SoundSourceDesc * _source, float _volume );

    protected:
        bool getSoundSourceDesc_( unsigned int _emitterId, SoundSourceDesc ** _desc );
        bool getSoundSourceDesc_( unsigned int _emitterId, const SoundSourceDesc ** _desc ) const;

	protected:
		SoundSystemInterface * m_soundSystem;

        ServiceProviderInterface * m_serviceProvider;

        float m_commonVolume;
		float m_soundVolume;		
		float m_musicVolume;
        float m_voiceVolume;

        unsigned int m_enumerator;

		struct SoundDesc
		{
			InputStreamInterface * stream;
			SoundDecoderInterface * codec;
		};

		typedef std::map<SoundBufferInterface*, SoundDesc> TMapBufferStreams;
		TMapBufferStreams m_bufferStreams;

		typedef std::map<unsigned int, SoundSourceDesc> TMapSoundSource;
		TMapSoundSource m_soundSourceMap;

		typedef std::vector<SoundNodeListenerInterface*> TVectorSourceListener;
		TVectorSourceListener m_stopListeners;
		TVectorSourceListener m_pauseListeners;

        bool m_silent;
		bool m_muted;

		bool m_turn;

	private:
		void playSounds_();
		void stopSounds_();
		void updateVolume_();
	};
};
