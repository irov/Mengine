#pragma once

#include "Interface/Interface.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AmplifierMusicCallbackInterface
        : public Interface
    {
    public:
        virtual void onMusicPause() = 0;
        virtual void onMusicResume() = 0;
        virtual void onMusicStop() = 0;
        virtual void onMusicEnd() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AmplifierMusicCallbackInterface> AmplifierMusicCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AmplifierInterface
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

        virtual void setPosMs( float _posMs ) = 0;
        virtual float getPosMs() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define AMPLIFIER_SERVICE()\
    ((Mengine::AmplifierInterface *)SERVICE_GET(Mengine::AmplifierInterface))
//////////////////////////////////////////////////////////////////////////