#pragma once

#include "Interface/ServiceInterface.h"

#import "Environment/Apple/AppleSemaphoreListenerInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class AppleNativePythonServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleNativePythonService" )

    public:
        virtual void waitSemaphore( const ConstString & _name, const AppleSemaphoreListenerInterfacePtr & _listener ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_NATIVEPYTHON_SERVICE()\
    ((AppleNativePythonServiceInterface *)SERVICE_GET(Mengine::AppleNativePythonServiceInterface))
//////////////////////////////////////////////////////////////////////////
