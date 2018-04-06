#pragma once

#include "Interface/StreamInterface.h"
#include "Interface/SoundSystemInterface.h"

#include "Core/ServiceBase.h"

#include "MixerVolume.h"

#include "ThreadWorkerSoundBufferUpdate.h"

#include "Core/ConstString.h"
#include "Factory/Factory.h"

#include	"math/vec3.h"

#include "stdex/stl_map.h"
#include "stdex/stl_vector.h"
#include "stdex/pool.h"

namespace Mengine
{
	enum ESoundSourceState
	{
		ESS_STOP = 0,	    // currently stopped
		ESS_PLAY = 2,		// currently playing
		ESS_PAUSE = 4,			// currently paused
	};

	struct SoundSourceDesc
	{
		uint32_t soundId;

		SoundSourceInterfacePtr source;
        SoundListenerInterfacePtr listener;

		ThreadWorkerSoundBufferUpdatePtr worker;
        uint32_t bufferId;

		float timing;
		MixerVolume volume;

		ESoundSourceState state;
		ESoundSourceType type;

        bool streamable;
		bool looped;
		bool turn;
	};

	class SoundEngine
		: public ServiceBase<SoundServiceInterface>
	{
	public:
		SoundEngine();
		~SoundEngine() override;

    public:
		bool _initialize() override;
		void _finalize() override;

    public:
        void _stopService() override;

	public:
		bool supportStreamSound() const override;

	public:
		void tick( float _time, float _timing ) override;

	public:
		void addSoundVolumeProvider( const SoundVolumeProviderInterfacePtr & _soundVolumeProvider ) override;
		bool removeSoundVolumeProvider(const SoundVolumeProviderInterfacePtr & _soundVolumeProvider ) override;

        
    public:
		void onTurnStream( bool _turn ) override;
        void onTurnSound( bool _turn ) override;

    public:
		uint32_t createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, ESoundSourceType _type, bool _streamable ) override;

    public:
		SoundBufferInterfacePtr createSoundBufferFromFile( const ConstString& _pakName, const FilePath & _filePath, const ConstString & _codecType, bool _streamable ) override;

	protected:
		SoundDecoderInterfacePtr createSoundDecoder_( const ConstString& _pakName, const FilePath & _filePath, const ConstString & _codecType, bool _streamable );

    public:
		void setSoundVolume( const ConstString & _type, float _volume, float _default ) override;
		float getSoundVolume( const ConstString & _type ) const override;
		float mixSoundVolume() const override;

		void setCommonVolume( const ConstString & _type, float _volume, float _default ) override;
		float getCommonVolume( const ConstString & _type ) const override;
		float mixCommonVolume() const override;

		void setMusicVolume( const ConstString & _type, float _volume, float _default ) override;
		float getMusicVolume( const ConstString & _type ) const override;
		float mixMusicVolume() const override;

		void setVoiceVolume( const ConstString & _type, float _volume, float _default ) override;
        float getVoiceVolume( const ConstString & _type ) const override;
		float mixVoiceVolume() const override;

	public:
		bool setSourceVolume( uint32_t _emitterId, float _volume, float _default ) override;
		float getSourceVolume( uint32_t _emitterId ) const override;

	public:
		bool setSourceMixerVolume( uint32_t _emitter, const ConstString & _mixer, float _volume, float _default ) override;
		float getSourceMixerVolume( uint32_t _emitter, const ConstString & _mixer ) const override;

	public:
		bool releaseSoundSource( uint32_t _sourceID ) override;

		bool validSoundSource( uint32_t _sourceID ) const override;
		
	public:
		bool playEmitter( uint32_t _emitterId ) override;
		bool pauseEmitter( uint32_t _emitterId ) override;
		bool resumeEmitter( uint32_t _emitterId ) override;
		bool stopEmitter( uint32_t _emitterId ) override;

	public:
		bool setLoop( uint32_t _emitterId, bool _looped ) override;
		bool getLoop( uint32_t _emitterId ) const override;

	public:
		void setSourceListener( uint32_t _emitterId, const SoundListenerInterfacePtr & _listener ) override;
		
	public:
		float getDuration( uint32_t _emitterId ) const override;
		
		bool setPosMs( uint32_t _emitterId, float _pos ) override;
		float getPosMs( uint32_t _emitterId ) override;

		void mute( bool _mute ) override;
		bool isMute() const override;

	public:
		void updateVolume() override;

    protected:
		void updateSoundVolumeProvider_( const SoundVolumeProviderInterfacePtr & _provider );
		void updateSourceVolume_( SoundSourceDesc * _source );

    protected:
        bool getSoundSourceDesc_( uint32_t _emitterId, SoundSourceDesc ** _desc );
        bool getSoundSourceDesc_( uint32_t _emitterId, const SoundSourceDesc ** _desc ) const;

    protected:
        void playSounds_();
        void pauseSounds_();
        void stopSounds_();      
        
    protected:
        bool stopSoundBufferUpdate_( SoundSourceDesc * _source );
        bool playSoundBufferUpdate_( SoundSourceDesc * _source );

	protected:
        MixerVolume m_commonVolume;
		MixerVolume m_soundVolume;
		MixerVolume m_musicVolume;
        MixerVolume m_voiceVolume;

        uint32_t m_enumerator;

        typedef stdex::template_pool<SoundSourceDesc, 32> TPoolSoundSourceDesc;
        TPoolSoundSourceDesc m_poolSoundSourceDesc;

		typedef stdex::map<uint32_t, SoundSourceDesc *> TMapSoundSource;
		TMapSoundSource m_soundSourceMap;
		
        ThreadJobPtr m_threadJobSoundBufferUpdate;

        FactoryPtr m_factoryWorkerTaskSoundBufferUpdate;

		typedef stdex::vector<SoundVolumeProviderInterfacePtr> TVectorSoundVolumeProviders;
		TVectorSoundVolumeProviders m_soundVolumeProviders;

		bool m_supportStream;
		bool m_muted;

		bool m_turnStream;
		bool m_turnSound;		
	};
};
