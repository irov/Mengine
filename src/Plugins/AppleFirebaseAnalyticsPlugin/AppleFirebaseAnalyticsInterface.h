#pragma once

#include "Interface/ServiceInterface.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class AppleFirebaseAnalyticsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebaseAnalyticsService" )

    public:
        virtual NSString * getkFIREventAdImpression() const = 0;        
        virtual NSString * getkFIRParameterAdPlatform() const = 0;
        virtual NSString * getkFIRParameterAdSource() const = 0;
        virtual NSString * getkFIRParameterAdFormat() const = 0;
        virtual NSString * getkFIRParameterAdUnitName() const = 0;
        virtual NSString * getkFIRParameterCurrency() const = 0;
        virtual NSString * getkFIRParameterValue() const = 0;
        
    public:
        virtual void sendEvent( NSString * _name, NSDictionary<NSString *, id> * parameters ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_ANALYTICS_SERVICE()\
    ((Mengine::AppleFirebaseAnalyticsServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseAnalyticsServiceInterface))
//////////////////////////////////////////////////////////////////////////
