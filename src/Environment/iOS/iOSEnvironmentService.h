#pragma once

#include "Interface/EnvironmentServiceInterface.h"
#include "Interface/LoggerInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class iOSEnvironmentService
        : public ServiceBase<EnvironmentServiceInterface>
    {
    public:
        iOSEnvironmentService();
        ~iOSEnvironmentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void getUserName( Char * const _userName ) const override;
        void getDeviceModel( Char * const _deviceModel ) const override;
        void getDeviceLanguage( Char * const _deviceLanguage ) const override;
        void getOSFamily( Char * const _osFamily ) const override;
        void getOSVersion( Char * const _osVersion ) const override;
        void getBundleId( Char * const _bundleId ) const override;
        
    public:
        void getFingerprint( Char * const _fingerprint ) const override;
        
    public:
        void getSessionId( Char * const _sessionId ) const override;
        void getInstallKey( Char * const _installKey ) const override;
        int64_t getInstallTimestamp() const override;
        void getInstallVersion( Char * const _installVersion ) const override;
        int64_t getInstallRND() const override;
        int64_t getSessionIndex() const override;
        
    protected:
        StaticString<MENGINE_ENVIRONMENT_USER_MAXNAME> m_userName;
        StaticString<MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME> m_deviceModel;
        StaticString<MENGINE_ENVIRONMENT_DEVICE_LANGUAGE_MAXNAME> m_deviceLanguage;
        StaticString<MENGINE_ENVIRONMENT_OS_FAMILY_MAXNAME> m_osFamily;
        StaticString<MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME> m_osVersion;
        StaticString<MENGINE_ENVIRONMENT_BUNDLEID_MAXNAME> m_bundleId;

        StaticString<MENGINE_ENVIRONMENT_FINGERPRINT_MAXNAME> m_fingerprint;

        StaticString<MENGINE_ENVIRONMENT_SESSIONID_MAXNAME> m_sessionId;
        StaticString<MENGINE_ENVIRONMENT_INSTALLKEY_MAXNAME> m_installKey;
        int64_t m_installTimestamp;
        StaticString<MENGINE_ENVIRONMENT_INSTALLVERSION_MAXNAME> m_installVersion;
        int64_t m_installRND;
        int64_t m_sessionIndex;
    };
}
