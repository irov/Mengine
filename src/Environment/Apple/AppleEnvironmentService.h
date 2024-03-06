#pragma once

#include "Interface/AppleEnvironmentServiceInterface.h"

#include "ConstStringHolderNSString.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Pool.h"
#include "Kernel/IntrusiveList.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class AppleEnvironmentService
        : public ServiceBase<AppleEnvironmentServiceInterface>
    {
    public:
        AppleEnvironmentService();
        ~AppleEnvironmentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        size_t getDeviceName( Char * _deviceName, size_t _capacity ) const override;
        size_t getDeviceModel( Char * _deviceModel, size_t _capacity ) const override;
        size_t getDeviceLanguage( Char * _deviceName, size_t _capacity ) const override;
        size_t getOSVersion( Char * _deviceName, size_t _capacity ) const override;
        size_t getBundleId( Char * _bundleId, size_t _capacity ) const override;
        
    public:
        void stringize( NSString * _value, ConstString * const _cstr ) override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;

    protected:
        typedef Pool<ConstStringHolderNSString, 256> PoolConstStringHolderNSString;
        PoolConstStringHolderNSString m_poolNSString;

        typedef IntrusiveList<ConstStringHolderNSString> IntrusiveListConstStringHolderNSString;
        IntrusiveListConstStringHolderNSString m_holdersNSString;
    };
}
