#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class AppleEnvironmentServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleEnvironmentService" )

    public:
        void stringize( NSString * _value, ConstString * const _cstr ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_ENVIRONMENT_SERVICE()\
    ((Mengine::AppleEnvironmentServiceInterface *)SERVICE_GET(Mengine::AppleEnvironmentServiceInterface))
//////////////////////////////////////////////////////////////////////////