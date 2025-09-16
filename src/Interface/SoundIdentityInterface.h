#pragma once

#include "Interface/SoundSourceInterface.h"
#include "Interface/ThreadWorkerInterface.h"
#include "Interface/MixerMultiplicativeInterface.h"

#include "Kernel/Mixin.h"

#include "Config/UniqueId.h"
#include "Config/Timestamp.h"

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
        ESS_INIT = 0,
        ESS_PLAY = 1,
        ESS_PAUSE = 2,
        ESS_STOP = 3,
        ESS_END = 4,
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class SoundIdentityInterface> SoundIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundListenerInterface
        : public Mixin
    {
    public:
        virtual void onSoundPlay( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual void onSoundPause( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual void onSoundResume( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual void onSoundStop( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual void onSoundEnd( const SoundIdentityInterfacePtr & _identity ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class SoundListenerInterface> SoundListenerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SoundIdentityInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const SoundSourceInterfacePtr & _source, ESoundSourceCategory _category, bool _streamable, bool _turn ) = 0;
        virtual void finalize() = 0;

    public:
        virtual const SoundSourceInterfacePtr & getSoundSource() const = 0;

    public:
        virtual void setSoundListener( const SoundListenerInterfacePtr & _listener ) = 0;
        virtual const SoundListenerInterfacePtr & getSoundListener() const = 0;
        virtual SoundListenerInterfacePtr popSoundListener() = 0;

    public:
        virtual void setWorkerUpdateBuffer( const ThreadWorkerInterfacePtr & _worker ) = 0;
        virtual const ThreadWorkerInterfacePtr & getWorkerUpdateBuffer() const = 0;

    public:
        virtual void setWorkerId( UniqueId _id ) = 0;
        virtual UniqueId getWorkerId() const = 0;

    public:
        virtual void setStreamable( bool _value ) = 0;
        virtual bool getStreamable() const = 0;

        virtual void setLoop( bool _loop ) = 0;
        virtual bool getLoop() const = 0;

        virtual void setTurn( bool _turn ) = 0;
        virtual bool getTurn() const = 0;

    public:
        virtual ESoundSourceCategory getCategory() const = 0;

    public:
        virtual void setState( ESoundSourceState _state ) = 0;
        virtual ESoundSourceState getState() const = 0;

    public:
        virtual Timestamp getStateTimestamp() const = 0;

    public:
        virtual void setTimeLeft( float _timeLeft ) = 0;
        virtual float getTimeLeft() const = 0;

    public:
        virtual const MixerMultiplicativeInterfacePtr & getMixerVolume() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundIdentityInterface> SoundIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}