#pragma once

#include "Interface/iOSKernelServiceInterface.h"
#include "Interface/LoggerInterface.h"

#import "Environment/iOS/iOSMailCompose.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class iOSKernelService
        : public ServiceBase<iOSKernelServiceInterface>
    {
    public:
        iOSKernelService();
        ~iOSKernelService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body ) override;
        
    protected:
        iOSMailCompose * m_mailCompose;

        LoggerInterfacePtr m_proxyLogger;
    };
}
