#pragma once

#include "AppleFirebaseCrashlyticsInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"

namespace Mengine
{
    class AppleFirebaseCrashlyticsService
        : public ServiceBase<AppleFirebaseCrashlyticsServiceInterface>
    {
    public:
        AppleFirebaseCrashlyticsService();
        ~AppleFirebaseCrashlyticsService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void sendValue( const ConstString & _val ) override;
        void sendKeyAndValue( const ConstString & _key, const ConstString & _val ) override;
        void sendKeyAndValues( const Params & _params ) override;
        void recordError( const String & _name, uint32_t _code ) override;
        void recordError( NSError * _error ) override;
    };
}
