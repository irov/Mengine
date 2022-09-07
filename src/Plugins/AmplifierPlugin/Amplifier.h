#pragma once

#include "AmplifierInterface.h"

#include "Interface/SoundIdentityInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Map.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class Amplifier
        : public ServiceBase<AmplifierInterface>
    {
    public:
        Amplifier();
        ~Amplifier() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void _stopService() override;

    public:
        bool playMusic( const ConstString & _resourceMusic, float _pos, bool _looped, const AmplifierMusicCallbackInterfacePtr & _callback ) override;

        void stopMusic() override;
        bool pauseMusic() override;
        bool resumeMusic() override;

    public:
        float getDuration() const override;

        void setPosMs( float _posMs ) override;
        float getPosMs() const override;

    protected:
        void notifyTurnSound_( bool _turn );

    protected:
        SoundIdentityInterfacePtr m_soundIdentity;

        bool m_play;
        bool m_pause;
    };
}
