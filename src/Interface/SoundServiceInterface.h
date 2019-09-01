#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/UpdationInterface.h"

#include "Interface/SoundIdentityInterface.h"
#include "Interface/SoundBufferInterface.h"
#include "Interface/SoundVolumeProviderInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SoundServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SoundService" )

    public:
        virtual void tick( const UpdateContext * _context ) = 0;

    public:
        virtual bool isSupportStreamSound() const = 0;

    public:
        virtual void addSoundVolumeProvider( const SoundVolumeProviderInterfacePtr & _soundVolumeProvider ) = 0;
        virtual bool removeSoundVolumeProvider( const SoundVolumeProviderInterfacePtr & _soundVolumeProvider ) = 0;

    public:
        virtual void onTurnStream( bool _turn ) = 0;
        virtual void onTurnSound( bool _turn ) = 0;

        virtual void updateVolume() = 0;

    public:
        virtual SoundIdentityInterfacePtr createSoundIdentity( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, ESoundSourceCategory _type, bool _streamable, const Char * _doc ) = 0;

    public:
        virtual SoundBufferInterfacePtr createSoundBufferFromFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, bool _isStream ) = 0;

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
        virtual bool playEmitter( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual bool pauseEmitter( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual bool resumeEmitter( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual bool stopEmitter( const SoundIdentityInterfacePtr & _identity ) = 0;

    public:
        virtual bool isEmitterStop( const SoundIdentityInterfacePtr & _identity ) const = 0;
        virtual bool isEmitterPlay( const SoundIdentityInterfacePtr & _identity ) const = 0;
        virtual bool isEmitterPause( const SoundIdentityInterfacePtr & _identity ) const = 0;

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
}
//////////////////////////////////////////////////////////////////////////
#define SOUND_SERVICE()\
    ((Mengine::SoundServiceInterface *)SERVICE_GET(Mengine::SoundServiceInterface))
//////////////////////////////////////////////////////////////////////////