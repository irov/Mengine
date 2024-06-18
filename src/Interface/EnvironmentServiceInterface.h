#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class EnvironmentServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "EnvironmentService" )

    public:
        virtual size_t getDeviceName( Char * _deviceName, size_t _capacity ) const = 0;
        virtual size_t getDeviceModel( Char * _deviceModel, size_t _capacity ) const = 0;
        virtual size_t getDeviceLanguage( Char * _deviceLanguage, size_t _capacity ) const = 0;
        virtual size_t getOSFamily( Char * _osVersion, size_t _capacity ) const = 0;
        virtual size_t getOSVersion( Char * _osVersion, size_t _capacity ) const = 0;
        virtual size_t getBundleId( Char * _bundleId, size_t _capacity ) const = 0;

    public:
        virtual size_t getSessionId( Char * _sessionId, size_t _capacity ) const = 0;
        virtual size_t getInstallKey( Char * const _installKey, size_t _capacity ) const = 0;
        virtual int64_t getInstallTimestamp() const = 0;
        virtual size_t getInstallVersion( Char * const _installVersion, size_t _capacity ) const = 0;
        virtual int64_t getInstallRND() const = 0;
        virtual int64_t getSessionIndex() const = 0;
        
    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;
        virtual bool openMail( const Char * _email, const Char * _subject, const Char * _body ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ENVIRONMENT_SERVICE()\
    ((Mengine::EnvironmentServiceInterface *)SERVICE_GET(Mengine::EnvironmentServiceInterface))
//////////////////////////////////////////////////////////////////////////
