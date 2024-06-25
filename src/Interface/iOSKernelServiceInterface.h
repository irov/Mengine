#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class iOSKernelServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "iOSKernelService" )

    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;
        virtual bool openMail( const Char * _email, const Char * _subject, const Char * _body ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define IOS_KERNEL_SERVICE()\
    ((Mengine::iOSKernelServiceInterface *)SERVICE_GET(Mengine::iOSKernelServiceInterface))
//////////////////////////////////////////////////////////////////////////
