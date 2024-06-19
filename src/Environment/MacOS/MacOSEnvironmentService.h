#pragma once

#include "Interface/EnvironmentServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class MacOSEnvironmentService
        : public ServiceBase<EnvironmentServiceInterface>
    {
    public:
        MacOSEnvironmentService();
        ~MacOSEnvironmentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        size_t getDeviceName( Char * _deviceName, size_t _capacity ) const override;
        size_t getDeviceModel( Char * _deviceModel, size_t _capacity ) const override;
        size_t getDeviceLanguage( Char * _deviceLanguage, size_t _capacity ) const override;
        size_t getOSFamily( Char * _osFamily, size_t _capacity ) const override;
        size_t getOSVersion( Char * _osVersion, size_t _capacity ) const override;
        size_t getBundleId( Char * _bundleId, size_t _capacity ) const override;
        
    public:
        size_t getSessionId( Char * _sessionId, size_t _capacity ) const override;
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
