#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Timestamp.h"
#include "Config/Char.h"

#ifndef MENGINE_ENVIRONMENT_USER_MAXNAME
#define MENGINE_ENVIRONMENT_USER_MAXNAME 256
#endif

#ifndef MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME
#define MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME 128
#endif

#ifndef MENGINE_ENVIRONMENT_DEVICE_LANGUAGE_MAXNAME
#define MENGINE_ENVIRONMENT_DEVICE_LANGUAGE_MAXNAME 64
#endif

#ifndef MENGINE_ENVIRONMENT_OS_FAMILY_MAXNAME
#define MENGINE_ENVIRONMENT_OS_FAMILY_MAXNAME 64
#endif

#ifndef MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME
#define MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME 64
#endif

#ifndef MENGINE_ENVIRONMENT_BUNDLEID_MAXNAME
#define MENGINE_ENVIRONMENT_BUNDLEID_MAXNAME 256
#endif

#ifndef MENGINE_ENVIRONMENT_INSTALLKEY_MAXNAME
#define MENGINE_ENVIRONMENT_INSTALLKEY_MAXNAME 128
#endif

#ifndef MENGINE_ENVIRONMENT_INSTALLVERSION_MAXNAME
#define MENGINE_ENVIRONMENT_INSTALLVERSION_MAXNAME 64
#endif

#ifndef MENGINE_ENVIRONMENT_SESSIONID_MAXNAME
#define MENGINE_ENVIRONMENT_SESSIONID_MAXNAME 32
#endif

namespace Mengine
{
    class EnvironmentServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "EnvironmentService" )

    public:
        virtual void getUserName( Char * const _userName ) const = 0;
        virtual void getDeviceModel( Char * const _deviceModel ) const = 0;
        virtual void getDeviceLanguage( Char * const _deviceLanguage ) const = 0;
        virtual void getOSFamily( Char * const _osVersion ) const = 0;
        virtual void getOSVersion( Char * const _osVersion ) const = 0;
        virtual void getBundleId( Char * const _bundleId ) const = 0;

    public:
        virtual void getSessionId( Char * const _sessionId ) const = 0;
        virtual void getInstallKey( Char * const _installKey ) const = 0;
        virtual Timestamp getInstallTimestamp() const = 0;
        virtual void getInstallVersion( Char * const _installVersion ) const = 0;
        virtual int64_t getInstallRND() const = 0;
        virtual Timestamp getSessionTimestamp() const = 0;
        virtual int64_t getSessionIndex() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ENVIRONMENT_SERVICE()\
    ((Mengine::EnvironmentServiceInterface *)SERVICE_GET(Mengine::EnvironmentServiceInterface))
//////////////////////////////////////////////////////////////////////////
