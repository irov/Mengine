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
        virtual size_t getDeviceName( Char * _deviceName, size_t _capacity ) const = 0;
        virtual size_t getDeviceModel( Char * _deviceModel, size_t _capacity ) const = 0;
        virtual size_t getDeviceLanguage( Char * _deviceName, size_t _capacity ) const = 0;
        virtual size_t getOSVersion( Char * _deviceName, size_t _capacity ) const = 0;
        virtual size_t getBundleId( Char * _bundleId, size_t _capacity ) const = 0;

    public:
        virtual size_t getSessionId( Char * _sessionId, size_t _capacity ) const = 0;
        virtual size_t getInstallKey( Char * const _installKey ) const = 0;
        virtual int64_t getInstallTimestamp() const = 0;
        virtual size_t getInstallVersion( Char * const _installVersion ) const = 0;
        virtual int64_t getInstallRND() const = 0;
        virtual int64_t getSessionIndex() const = 0;
        
    public:
        virtual void stringize( NSString * _value, ConstString * const _cstr ) = 0;

    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_ENVIRONMENT_SERVICE()\
    ((Mengine::AppleEnvironmentServiceInterface *)SERVICE_GET(Mengine::AppleEnvironmentServiceInterface))
//////////////////////////////////////////////////////////////////////////
