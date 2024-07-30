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
        bool getValueBoolean( const ConstString & _key ) const override;
        int64_t getValueInteger( const ConstString & _key ) const override;
        double getValueDouble( const ConstString & _key ) const override;
        ConstString getValueConstString( const ConstString & _key ) const override;
        String getValueJSON( const ConstString & _key ) const override;
        
        Params getValues() const override;
    };
}
