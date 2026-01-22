#pragma once

#include "Interface/AmplifierServiceInterface.h"

#include "Interface/SoundIdentityInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Map.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class AmplifierService
        : public ServiceBase<AmplifierServiceInterface>
    {
        DECLARE_FACTORABLE( AmplifierService );

    public:
        AmplifierService();
        ~AmplifierService() override;

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

        void setPosition( float _position ) override;
        float getPosition() const override;

    protected:
        SoundIdentityInterfacePtr m_soundIdentity;
    };
}
