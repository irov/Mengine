#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

#if defined(__OBJC__)
#import <Foundation/Foundation.h>
#else
class NSString;
#endif

namespace Mengine
{
    class AppleEnvironmentServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleEnvironmentService" )
        
    public:
        virtual void stringize( NSString * _value, ConstString * const _cstr ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_ENVIRONMENT_SERVICE()\
    ((Mengine::AppleEnvironmentServiceInterface *)SERVICE_GET(Mengine::AppleEnvironmentServiceInterface))
//////////////////////////////////////////////////////////////////////////
