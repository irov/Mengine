#pragma once

#include "AppleFirebaseRemoteConfigInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleFirebaseRemoteConfigService
        : public ServiceBase<AppleFirebaseRemoteConfigServiceInterface>
    {
    public:
        AppleFirebaseRemoteConfigService();
        ~AppleFirebaseRemoteConfigService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setRemoteConfig( NSMutableDictionary * _remoteConfig ) override;
        NSMutableDictionary * getRemoteConfig() const override;
        
    protected:
        NSMutableDictionary * m_remoteConfig;
    };
}
