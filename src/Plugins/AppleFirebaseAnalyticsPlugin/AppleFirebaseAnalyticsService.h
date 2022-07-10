#pragma once

#include "AppleFirebaseAnalyticsInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class AppleFirebaseAnalyticsService
        : public ServiceBase<AppleFirebaseAnalyticsServiceInterface>
    {
    public:
        AppleFirebaseAnalyticsService();
        ~AppleFirebaseAnalyticsService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void sendEvent( const ConstString & _name, const FirebaseAnalyticsParams & _params ) override;
    };
}
