#pragma once

#include "AppleFirebaseMessagingInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleFirebaseMessagingService
        : public ServiceBase<AppleFirebaseMessagingServiceInterface>
    {
    public:
        AppleFirebaseMessagingService();
        ~AppleFirebaseMessagingService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setPushToken( NSString * _pushToken ) override;
        NSString * getPushToken() const override;
        
    protected:
        NSString * m_pushToken;
    };
}
