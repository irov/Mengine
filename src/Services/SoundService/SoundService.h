#pragma once

#include "Interface/SoundServiceInterface.h"
#include "Interface/TimepipeInterface.h"
#include "Interface/FactoryInterface.h"

#include "SoundIdentity.h"
#include "ThreadWorkerSoundBufferUpdate.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"

#include "math/vec3.h"

namespace Mengine
{
    ////////////////////////////////////////////////////////////////////////
    class SoundService
        : public ServiceBase<SoundServiceInterface>
        , public TimepipeInterface
    {
    public:
        SoundService();
        ~SoundService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void _stopService() override;

    public:
        bool isSupportStreamSound() const override;

    public:
        void onTimepipe( const UpdateContext * _context ) override;

    public:
        void addSoundVolumeProvider( const SoundVolumeProviderInterfacePtr & _soundVolumeProvider ) override;
        bool removeSoundVolumeProvider( const SoundVolumeProviderInterfacePtr & _soundVolumeProvider ) override;

    public:
        void onTurnStream( bool _turn ) override;
        void onTurnSound( bool _turn ) override;

    public:
        SoundIdentityInterfacePtr createSoundIdentity( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, ESoundSourceCategory _category, bool _streamable, const DocumentInterfacePtr & _doc ) override;

    public:
        SoundBufferInterfacePtr createSoundBufferFromFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, bool _streamable, const DocumentInterfacePtr & _doc ) override;

    protected:
        SoundDecoderInterfacePtr createSoundDecoder_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, bool _streamable, const DocumentInterfacePtr & _doc );

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
        bool setSourceVolume( const SoundIdentityInterfacePtr & _identity, float _volume, float _default, bool _force ) override;
        float getSourceVolume( const SoundIdentityInterfacePtr & _identity ) const override;

    public:
        bool setSourceMixerVolume( const SoundIdentityInterfacePtr & _identity, const ConstString & _mixer, float _volume, float _default ) override;
        float getSourceMixerVolume( const SoundIdentityInterfacePtr & _identity, const ConstString & _mixer ) const override;

    public:
        bool releaseSoundSource( const SoundIdentityInterfacePtr & _identity ) override;

    public:
        bool playEmitter( const SoundIdentityInterfacePtr & _identity ) override;
        bool pauseEmitter( const SoundIdentityInterfacePtr & _identity ) override;
        bool resumeEmitter( const SoundIdentityInterfacePtr & _identity ) override;
        bool stopEmitter( const SoundIdentityInterfacePtr & _identity ) override;

    public:
        bool isEmitterStop( const SoundIdentityInterfacePtr & _identity ) const override;
        bool isEmitterPlay( const SoundIdentityInterfacePtr & _identity ) const override;
        bool isEmitterPause( const SoundIdentityInterfacePtr & _identity ) const override;

    public:
        bool setLoop( const SoundIdentityInterfacePtr & _identity, bool _looped ) override;
        bool getLoop( const SoundIdentityInterfacePtr & _identity ) const override;

    public:
        float getDuration( const SoundIdentityInterfacePtr & _identity ) const override;

        bool setPosMs( const SoundIdentityInterfacePtr & _identity, float _pos ) override;
        float getPosMs( const SoundIdentityInterfacePtr & _identity ) override;

        void mute( bool _mute ) override;
        bool isMute() const override;

    public:
        void updateVolume() override;

    protected:
        void notifyEnginePrepareFinalize();

    protected:
        void updateSoundVolumeProvider_( const SoundVolumeProviderInterfacePtr & _provider );
        void updateSourceVolume_( const SoundIdentityPtr & _source );

    protected:
        void playSounds_();
        void stopSounds_();
        void pauseSounds_();
        void resumeSounds_();

    protected:
        bool playSoundBufferUpdate_( const SoundIdentityPtr & _source );
        bool stopSoundBufferUpdate_( const SoundIdentityPtr & _source );
        bool pauseSoundBufferUpdate_( const SoundIdentityPtr & _source );
        bool resumeSoundBufferUpdate_( const SoundIdentityPtr & _source );

    protected:
        bool checkMaxSoundPlay_() const;

    protected:
        UniqueId m_timepipeId;

        MixerValue m_commonVolume;
        MixerValue m_soundVolume;
        MixerValue m_musicVolume;
        MixerValue m_voiceVolume;

        FactoryInterfacePtr m_factorySoundIdentity;

        typedef Vector<SoundIdentityPtr> VectorSoundIdentity;
        VectorSoundIdentity m_soundIdentities;
        VectorSoundIdentity m_soundIdentitiesAux;

        ThreadJobPtr m_threadJobSoundBufferUpdate;

        FactoryInterfacePtr m_factoryWorkerTaskSoundBufferUpdate;

        typedef Vector<SoundVolumeProviderInterfacePtr> VectorSoundVolumeProviders;
        VectorSoundVolumeProviders m_soundVolumeProviders;

        bool m_supportStream;
        bool m_muted;

        bool m_turnStream;
        bool m_turnSound;
    };
};
