#	pragma once

#   include "Interface/StreamInterface.h"
#	include "Interface/SoundSystemInterface.h"

#	include "MixerVolume.h"

#   include "Kernel/ThreadJob.h"
#   include "ThreadWorkerSoundBufferUpdate.h"

#	include "Core/ConstString.h"
#   include "Factory/FactoryPool.h"

#	include	"math/vec3.h"

#   include "stdex/binary_vector.h"

#	include <vector>

namespace Menge
{
	enum ESoundSourceState
	{
		ESS_STOPPED = 0,	    // currently stopped
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
		SoundSourceInterface * soundSourceInterface;
		SoundNodeListenerInterface * listener;
		ThreadWorkerSoundBufferUpdate * taskSoundBufferUpdate;
        size_t taskSoundBufferUpdateId;

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
		void setSoundVolume( const ConstString & _type, float _volume ) override;
		float getSoundVolume( const ConstString & _type ) const override;

		void setCommonVolume( const ConstString & _type, float _volume ) override;
		float getCommonVolume( const ConstString & _type ) const override;

		void setMusicVolume( const ConstString & _type, float _volume ) override;
		float getMusicVolume( const ConstString & _type ) const override;

        void setVoiceVolume( const ConstString & _type, float _volume ) override;
        float getVoiceVolume( const ConstString & _type ) const override;

	public:
		bool setSourceVolume( unsigned int _emitter, float _volume ) override;
		float getSourceVolume( unsigned int _emitter ) const override;

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
		
		bool setPosMs( unsigned int _emitter, float _pos ) override;
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
        void playSounds_();
        void pauseSounds_();
        void stopSounds_();        
        void updateVolume_();

    protected:
        void stopSoundBufferUpdate_( SoundSourceDesc * _source );
        void playSoundBufferUpdate_( SoundSourceDesc * _source );

	protected:
        ServiceProviderInterface * m_serviceProvider;

        MixerVolume m_commonVolume;
		MixerVolume m_soundVolume;
		MixerVolume m_musicVolume;
        MixerVolume m_voiceVolume;

        unsigned int m_enumerator;

		struct SoundDesc
		{
			InputStreamInterfacePtr stream;
			SoundDecoderInterfacePtr codec;
		};

        typedef stdex::template_pool<SoundSourceDesc, 32> TPoolSoundSourceDesc;
        TPoolSoundSourceDesc m_poolSoundSourceDesc;

		typedef stdex::binary_vector<unsigned int, SoundSourceDesc *> TMapSoundSource;
		TMapSoundSource m_soundSourceMap;

        ThreadJob m_threadSoundBufferUpdate;

        typedef FactoryPool<ThreadWorkerSoundBufferUpdate, 32> TPoolWorkerTaskSoundBufferUpdate;
        TPoolWorkerTaskSoundBufferUpdate m_poolWorkerTaskSoundBufferUpdate;

		typedef std::vector<SoundNodeListenerInterface*> TVectorSourceListener;
		TVectorSourceListener m_stopListeners;
		TVectorSourceListener m_pauseListeners;

        bool m_silent;
		bool m_muted;

		bool m_turn;
	};
};
