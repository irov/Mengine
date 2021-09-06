#pragma once

#include "Interface/SoundSourceInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/MixerValue.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESoundSourceCategory
    {
        ES_SOURCE_CATEGORY_SOUND,
        ES_SOURCE_CATEGORY_MUSIC,
        ES_SOURCE_CATEGORY_VOICE,
    };
    //////////////////////////////////////////////////////////////////////////
    enum ESoundSourceState
    {
        ESS_STOP = 0,
        ESS_PLAY = 2,
        ESS_PAUSE = 4,
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class SoundIdentityInterface> SoundIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundListenerInterface
        : public Mixin
    {
    public:
        virtual void onSoundPause( const SoundIdentityInterfacePtr & _emitter ) = 0;
        virtual void onSoundResume( const SoundIdentityInterfacePtr & _emitter ) = 0;
        virtual void onSoundStop( const SoundIdentityInterfacePtr & _emitter ) = 0;
        virtual void onSoundEnd( const SoundIdentityInterfacePtr & _emitter ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class SoundListenerInterface> SoundListenerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundIdentityInterface
        : public Mixin
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual UniqueId getId() const = 0;

    public:
        virtual void setSoundSource( const SoundSourceInterfacePtr & _source ) = 0;
        virtual const SoundSourceInterfacePtr & getSoundSource() const = 0;

        virtual void setSoundListener( const SoundListenerInterfacePtr & _listener ) = 0;
        virtual const SoundListenerInterfacePtr & getSoundListener() const = 0;
        virtual SoundListenerInterfacePtr popSoundListener() = 0;

    public:
        virtual bool isStreamable() const = 0;
        virtual bool getLoop() const = 0;

    public:
        virtual ESoundSourceCategory getCategory() const = 0;
        virtual ESoundSourceState getState() const = 0;

    public:
        virtual const MixerValue & getVolume() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundIdentityInterface> SoundIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}