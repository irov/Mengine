#pragma once

#include "Interface/EnvironmentServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/StaticString.h"

namespace Mengine
{
    class AndroidEnvironmentService
        : public ServiceBase<EnvironmentServiceInterface>
    {
    public:
        AndroidEnvironmentService();
        ~AndroidEnvironmentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void getUserName( Char * const _deviceName ) const override;
        void getDeviceModel( Char * const _deviceName ) const override;
        void getDeviceLanguage( Char * const _deviceName ) const override;
        void getOSFamily( Char * const _osFamily ) const override;
        void getOSVersion( Char * const _osVersion ) const override;
        void getBundleId( Char * const _bundleId ) const override;

    public:
        void getInstallKey( Char * const _installKey ) const override;
        Timestamp getInstallTimestamp() const override;
        void getInstallVersion( Char * const _installVersion ) const override;
        int64_t getInstallRND() const override;
        int64_t getSessionIndex() const override;
        Timestamp getSessionTimestamp() const override;
        int64_t getSessionRND() const override;
        void getSessionId( Char * const _sessionId ) const override;

    protected:
        StaticString<MENGINE_ENVIRONMENT_USER_MAXNAME> m_userName;
        StaticString<MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME> m_deviceModel;
        StaticString<MENGINE_ENVIRONMENT_DEVICE_LANGUAGE_MAXNAME> m_deviceLanguage;
        StaticString<MENGINE_ENVIRONMENT_OS_FAMILY_MAXNAME> m_osFamily;
        StaticString<MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME> m_osVersion;
        StaticString<MENGINE_ENVIRONMENT_BUNDLEID_MAXNAME> m_bundleId;

        StaticString<MENGINE_ENVIRONMENT_SESSIONID_MAXNAME> m_sessionId;
        StaticString<MENGINE_ENVIRONMENT_INSTALLKEY_MAXNAME> m_installKey;
        Timestamp m_installTimestamp;
        StaticString<MENGINE_ENVIRONMENT_INSTALLVERSION_MAXNAME> m_installVersion;
        int64_t m_installRND;
        Timestamp m_sessionTimestamp;
        int64_t m_sessionRND;
        int64_t m_sessionIndex;
    };
}
