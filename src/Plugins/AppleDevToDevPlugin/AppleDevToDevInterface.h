#pragma once

#include "Interface/ServiceInterface.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class AppleDevToDevInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleDevToDevService" )

    public:
        virtual void sendEvent( NSString * _eventName, NSDictionary<NSString *, id> * parameters ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_DEVTODEV_SERVICE()\
    ((Mengine::AppleDevToDevInterface *)SERVICE_GET(Mengine::AppleDevToDevInterface))
//////////////////////////////////////////////////////////////////////////
