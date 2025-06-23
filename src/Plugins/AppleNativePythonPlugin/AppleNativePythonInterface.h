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
        virtual pybind::object addAppleCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb, const pybind::args & _args ) = 0;
        virtual void removeAppleCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb ) = 0;

        
    public:
        virtual void activateSemaphore( const ConstString & _name ) = 0;
        virtual AppleSemaphoreListenerInterfacePtr waitSemaphore( const ConstString & _name, const AppleSemaphoreListenerInterfacePtr & _listener ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_NATIVEPYTHON_SERVICE()\
    ((AppleNativePythonServiceInterface *)SERVICE_GET(Mengine::AppleNativePythonServiceInterface))
//////////////////////////////////////////////////////////////////////////
