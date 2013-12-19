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
		ESS_STOP = 0,	    // currently stopped
		ESS_PLAY = 2,		// currently playing
		ESS_PAUSE = 4,			// currently paused
	};

	struct SoundSourceDesc
	{
		size_t soundId;

		SoundSourceInterface * source;
		SoundListenerInterface * listener;

		ThreadWorkerSoundBufferUpdate * worker;
        size_t bufferId;

		float timing;
		float volume;

		ESoundSourceState state;
		ESoundSourceType type;

        bool streamable;
		bool looped;
		bool turn;
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
		size_t createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample, ESoundSourceType _type, bool _streamable ) override;

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
		bool setSourceVolume( size_t _emitter, float _volume ) override;
		float getSourceVolume( size_t _emitter ) const override;

		void releaseSoundSource( size_t _sourceID ) override;

		bool validSoundSource( size_t _sourceID ) const override;
		
	public:
		bool play( size_t _emitter ) override;
		bool pause( size_t _emitter ) override;
		bool stop( size_t _emitter ) override;

	public:
		void setLoop( size_t _emitter, bool _looped ) override;
		bool getLoop( size_t _emitter ) const override;

	public:
		void setSourceListener( size_t _emitter, SoundListenerInterface* _listener ) override;
		
	public:
		float getLengthMs( size_t _emitter ) const override;
		
		bool setPosMs( size_t _emitter, float _pos ) override;
		float getPosMs( size_t _emitter ) const override;

		void mute( bool _mute ) override;
		bool isMute() const override;

        bool isSilent() const override;

    protected:
        void updateSourceVolume_( SoundSourceDesc * _source, float _volume );

    protected:
        bool getSoundSourceDesc_( size_t _emitterId, SoundSourceDesc ** _desc );
        bool getSoundSourceDesc_( size_t _emitterId, const SoundSourceDesc ** _desc ) const;

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

        size_t m_enumerator;

		struct SoundDesc
		{
			InputStreamInterfacePtr stream;
			SoundDecoderInterfacePtr codec;
		};

        typedef stdex::template_pool<SoundSourceDesc, 32> TPoolSoundSourceDesc;
        TPoolSoundSourceDesc m_poolSoundSourceDesc;

		typedef stdex::binary_vector<size_t, SoundSourceDesc *> TMapSoundSource;
		TMapSoundSource m_soundSourceMap;
		
        ThreadJob m_threadSoundBufferUpdate;

        typedef FactoryPool<ThreadWorkerSoundBufferUpdate, 32> TPoolWorkerTaskSoundBufferUpdate;
        TPoolWorkerTaskSoundBufferUpdate m_poolWorkerTaskSoundBufferUpdate;

        bool m_silent;
		bool m_muted;

		bool m_turn;
	};
};
