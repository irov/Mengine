#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/SoundIdentityInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AmplifierMusicCallbackInterface
        : public Mixin
    {
    public:
        virtual void onMusicPause( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual void onMusicResume( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual void onMusicStop( const SoundIdentityInterfacePtr & _identity ) = 0;
        virtual void onMusicEnd( const SoundIdentityInterfacePtr & _identity ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AmplifierMusicCallbackInterface> AmplifierMusicCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AmplifierServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Amplifier" )

    public:
        virtual bool playMusic( const ConstString & _resourceMusic, float _pos, bool _looped, const AmplifierMusicCallbackInterfacePtr & _callback ) = 0;

        virtual void stopMusic() = 0;
        virtual bool pauseMusic() = 0;
        virtual bool resumeMusic() = 0;

    public:
        virtual float getDuration() const = 0;

        virtual void setPosition( float _position ) = 0;
        virtual float getPosition() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define AMPLIFIER_SERVICE()\
    ((Mengine::AmplifierServiceInterface *)SERVICE_GET(Mengine::AmplifierServiceInterface))
//////////////////////////////////////////////////////////////////////////