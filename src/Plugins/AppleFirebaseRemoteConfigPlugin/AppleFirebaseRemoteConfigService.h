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
        bool hasRemoteConfig( const ConstString & _key ) const override;
        bool getRemoteConfigValue( const ConstString & _key, Params * const _params ) const override;
    };
}
