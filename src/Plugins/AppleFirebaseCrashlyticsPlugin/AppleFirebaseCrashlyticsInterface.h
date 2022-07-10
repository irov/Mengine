#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"
#include "Kernel/Map.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Map<ConstString, ConstString> FirebaseCrashlyticsParams;
    //////////////////////////////////////////////////////////////////////////
    class AppleFirebaseCrashlyticsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFirebaseCrashlyticsService" )

    public:
        virtual void sendValue( const ConstString & _val ) = 0;
        virtual void sendKeyAndValue( const ConstString & _key, const ConstString & _val ) = 0;
        virtual void sendKeyAndValues( const FirebaseCrashlyticsParams & _params ) = 0;
        virtual void recordError( const ConstString & _name, long _code, const FirebaseCrashlyticsParams & _params ) = 0;
        virtual void recordError( NSError * _error ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FIREBASE_CRASHLYTICS_SERVICE()\
    ((Mengine::AppleFirebaseCrashlyticsServiceInterface *)SERVICE_GET(Mengine::AppleFirebaseCrashlyticsServiceInterface))
//////////////////////////////////////////////////////////////////////////
