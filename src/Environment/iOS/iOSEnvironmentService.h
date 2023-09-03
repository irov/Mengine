#pragma once

#include "Interface/iOSEnvironmentServiceInterface.h"

#include "Kernel/ServiceBase.h"

#import "Environment/iOS/iOSMailCompose.h"

namespace Mengine
{
    class iOSEnvironmentService
        : public ServiceBase<iOSEnvironmentServiceInterface>
    {
    public:
        iOSEnvironmentService();
        ~iOSEnvironmentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body ) override;
        
    protected:
        iOSMailCompose * m_mailCompose;
    };
}
