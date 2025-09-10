#pragma once

#include "Interface/AmplifierServiceInterface.h"

#include "Interface/SoundIdentityInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Map.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class AmplifierSoundListener
        : public Factorable
        , public SoundListenerInterface
    {
        DECLARE_FACTORABLE( AmplifierSoundListener );

    public:
        explicit AmplifierSoundListener( const AmplifierMusicCallbackInterfacePtr & _callback );
        ~AmplifierSoundListener() override;

    protected:
        void onSoundPlay( const SoundIdentityInterfacePtr & _identity ) override;
        void onSoundPause( const SoundIdentityInterfacePtr & _identity ) override;
        void onSoundResume( const SoundIdentityInterfacePtr & _identity ) override;
        void onSoundStop( const SoundIdentityInterfacePtr & _identity ) override;
        void onSoundEnd( const SoundIdentityInterfacePtr & _identity ) override;

    protected:
        AmplifierMusicCallbackInterfacePtr m_callback;
    };
}
