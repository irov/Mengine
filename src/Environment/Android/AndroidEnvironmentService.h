#pragma once

#include "Interface/EnvironmentServiceInterface.h"

#include "Kernel/ServiceBase.h"

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
        size_t getDeviceName( Char * const _deviceName, size_t _capacity ) const override;
        size_t getDeviceModel( Char * const _deviceName, size_t _capacity ) const override;
        size_t getDeviceLanguage( Char * const _deviceName, size_t _capacity ) const override;
        size_t getOSFamily( Char * _osVersion, size_t _capacity ) const override;
        size_t getOSVersion( Char * const _deviceName, size_t _capacity ) const override;
        size_t getBundleId( Char * const _packageName, size_t _capacity ) const override;

    public:
        size_t getSessionId( Char * const _sessionId, size_t _capacity ) const override;
        size_t getInstallKey( Char * const _installKey, size_t _capacity ) const override;
        int64_t getInstallTimestamp() const override;
        size_t getInstallVersion( Char * const _installVersion, size_t _capacity ) const override;
        int64_t getInstallRND() const override;
        int64_t getSessionIndex() const override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body ) override;
    };
}
