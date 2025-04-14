#pragma once

#include "Interface/EnvironmentServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/StaticString.h"

namespace Mengine
{
    class Win32EnvironmentService
        : public ServiceBase<EnvironmentServiceInterface>
    {
    public:
        Win32EnvironmentService();
        ~Win32EnvironmentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void getUserName( Char * const _userName ) const override;
        void getDeviceModel( Char * const _deviceModel ) const override;
        void getDeviceLanguage( Char * const _deviceLanguage ) const override;
        void getOSFamily( Char * const _osVersion ) const override;
        void getOSVersion( Char * const _osVersion ) const override;
        void getBundleId( Char * const _bundleId ) const override;

    public:
        void getInstallKey( Char * const _installKey ) const override;
        Timestamp getInstallTimestamp() const override;
        void getInstallVersion( Char * const _installVersion ) const override;
        int64_t getInstallRND() const override;
        int64_t getSessionIndex() const override;
        Timestamp getSessionTimestamp() const override;
        void getSessionId( Char * const _sessionId ) const override;


    protected:
        StaticString<MENGINE_ENVIRONMENT_USER_MAXNAME> m_userName;
        StaticString<MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME> m_deviceModel;
        StaticString<MENGINE_ENVIRONMENT_DEVICE_LANGUAGE_MAXNAME> m_deviceLanguage;
        StaticString<MENGINE_ENVIRONMENT_OS_FAMILY_MAXNAME> m_osFamily;
        StaticString<MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME> m_osVersion;
        StaticString<MENGINE_ENVIRONMENT_BUNDLEID_MAXNAME> m_bundleId;

        StaticString<MENGINE_ENVIRONMENT_INSTALLKEY_MAXNAME> m_installKey;
        Timestamp m_installTimestamp;
        StaticString<MENGINE_ENVIRONMENT_INSTALLVERSION_MAXNAME> m_installVersion;
        int64_t m_installRND;
        int64_t m_sessionIndex;
        Timestamp m_sessionTimestamp;
        StaticString<MENGINE_ENVIRONMENT_SESSIONID_MAXNAME> m_sessionId;
    };
}
