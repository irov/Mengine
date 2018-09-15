#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/SoundCodecInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/UpdationInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/MixerValue.h"

#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESoundSourceType
    {
        ESST_SOUND,
        ESST_MUSIC,
        ESST_VOICE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ESoundSourceState
    {
        ESS_STOP = 0,	    // currently stopped
        ESS_PLAY = 2,		// currently playing
        ESS_PAUSE = 4,			// currently paused
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class SoundIdentityInterface> SoundIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundListenerInterface
        : public Factorable
    {
    public:
        virtual void onSoundPause( const SoundIdentityInterfacePtr & _emitter ) = 0;
        virtual void onSoundStop( const SoundIdentityInterfacePtr & _emitter ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundListenerInterface> SoundListenerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundBufferInterface
        : public ServantInterface
    {
    public:
        virtual bool update() = 0;

    public:
        virtual const SoundDecoderInterfacePtr & getDecoder() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundBufferInterface> SoundBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundSourceInterface
        : public ServantInterface
    {
    public:
        virtual bool play() = 0;
        virtual void pause() = 0;
        virtual void stop() = 0;

        virtual bool isPlay() const = 0;
        virtual bool isPause() const = 0;

        virtual void setVolume( float _volume ) = 0;
        virtual float getVolume() const = 0;

        virtual void setLoop( bool _loop ) = 0;
        virtual bool getLoop() const = 0;

        virtual float getDuration() const = 0;
        virtual float getPosition() const = 0;
        virtual bool setPosition( float _posMs ) = 0;

        virtual void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) = 0;
        virtual SoundBufferInterfacePtr getSoundBuffer() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundSourceInterface> SoundSourceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundIdentityInterface
        : public ServantInterface
    {
    public:
        virtual uint32_t getId() const = 0;

    public:
        virtual void setSoundSource( const SoundSourceInterfacePtr & _source ) = 0;
        virtual const SoundSourceInterfacePtr & getSoundSource() const = 0;

        virtual void setSoundListener( const SoundListenerInterfacePtr & _listener ) = 0;
        virtual const SoundListenerInterfacePtr & getSoundListener() const = 0;

    public:
        virtual bool isStreamable() const = 0;
        virtual bool getLoop() const = 0;

    public:
        virtual ESoundSourceType getType() const = 0;
        virtual ESoundSourceState getState() const = 0;

        virtual const MixerValue & getVolume() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundIdentityInterface> SoundIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SoundSystem" )

    public:
        virtual void update() = 0;

    public:
        virtual bool isSilent() const = 0;

    public:
        virtual void onTurnSound( bool _turn ) = 0;

    public:
        virtual SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable ) = 0;

    public:
        virtual SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define SOUND_SYSTEM()\
    ((Mengine::SoundSystemInterface *)SERVICE_GET(Mengine::SoundSystemInterface))
    //////////////////////////////////////////////////////////////////////////
    class SoundVolumeProviderInterface
        : public ServantInterface
    {
    public:
        virtual void onSoundChangeVolume( float _sound, float _music, float _voice, bool _mute ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundVolumeProviderInterface> SoundVolumeProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SoundService" )

    public:
        virtual void tick( const UpdateContext * _context ) = 0;

    public:
        virtual bool supportStreamSound() const = 0;

    public:
        virtual void addSoundVolumeProvider( const SoundVolumeProviderInterfacePtr & _soundVolumeProvider ) = 0;
        virtual bool removeSoundVolumeProvider( const SoundVolumeProviderInterfacePtr & _soundVolumeProvider ) = 0;

    public:
        virtual void onTurnStream( bool _turn ) = 0;
        virtual void onTurnSound( bool _turn ) = 0;

        virtual void updateVolume() = 0;

    public:
        virtual SoundIdentityInterfacePtr createSoundIdentity( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, ESoundSourceType _type, bool _streamable ) = 0;

    public:
        virtual SoundBufferInterfacePtr createSoundBufferFromFile( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, const ConstString & _codecType, bool _isStream ) = 0;

    public:
        virtual void setSoundVolume( const ConstString & _type, float _volume, float _default ) = 0;
        virtual float getSoundVolume( const ConstString & _type ) const = 0;
        virtual float mixSoundVolume() const = 0;

        virtual void setCommonVolume( const ConstString & _type, float _volume, float _default ) = 0;
        virtual float getCommonVolume( const ConstString & _type ) const = 0;
        virtual float mixCommonVolume() const = 0;

        virtual void setMusicVolume( const ConstString & _type, float _volume, float _default ) = 0;
        virtual float getMusicVolume( const ConstString & _type ) const = 0;
        virtual float mixMusicVolume() const = 0;

        virtual void setVoiceVolume( const ConstString & _type, float _volume, float _default ) = 0;
        virtual float getVoiceVolume( const ConstString & _type ) const = 0;
        virtual float mixVoiceVolume() const = 0;

    public:
        virtual bool setSourceVolume( const SoundIdentityInterfacePtr & _identity, float _volume, float _default, bool _force ) = 0;
        virtual float getSourceVolume( const SoundIdentityInterfacePtr & _identity ) const = 0;

    public:
        virtual bool setSourceMixerVolume( const SoundIdentityInterfacePtr & _identity, const ConstString & _mixer, float _volume, float _default ) = 0;
        virtual float getSourceMixerVolume( const SoundIdentityInterfacePtr & _identity, const ConstString & _mixer ) const = 0;

    public:
        virtual bool releaseSoundSource( const SoundIdentityInterfacePtr & _identity ) = 0;

    public:
        virtual void setSourceListener( const SoundIdentityInterfacePtr & _identity, const SoundListenerInterfacePtr & _listener ) = 0;

    public:
        virtual bool playEmitter( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual bool pauseEmitter( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual bool resumeEmitter( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual bool stopEmitter( const SoundIdentityInterfacePtr & _identity ) = 0;

    public:
        virtual bool setLoop( const SoundIdentityInterfacePtr & _identity, bool _looped ) = 0;
        virtual bool getLoop( const SoundIdentityInterfacePtr & _identity ) const = 0;

    public:
        virtual float getDuration( const SoundIdentityInterfacePtr & _identity ) const = 0;

        virtual bool setPosMs( const SoundIdentityInterfacePtr & _identity, float _pos ) = 0;
        virtual float getPosMs( const SoundIdentityInterfacePtr & _identity ) = 0;

    public:
        virtual void mute( bool _mute ) = 0;
        virtual bool isMute() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define SOUND_SERVICE()\
    ((Mengine::SoundServiceInterface *)SERVICE_GET(Mengine::SoundServiceInterface))
}