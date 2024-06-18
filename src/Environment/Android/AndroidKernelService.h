#pragma once

#include "Interface/AndroidKernelServiceInterface.h"
#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/LoggerInterface.h"

#include "ConstStringHolderJString.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Pool.h"
#include "Kernel/IntrusiveList.h"

namespace Mengine
{
    class AndroidKernelService
        : public ServiceBase<AndroidKernelServiceInterface>
        , public AnalyticsEventProviderInterface
    {
    public:
        AndroidKernelService();
        ~AndroidKernelService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        size_t getAndroidId( Char * _androidId, size_t _capacity ) const override;

    public:
        void stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr ) override;

    protected:
        void addAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;
        void removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;
        void invokeAndroidEventations() override;

    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;

    protected:
        void onAnalyticsFlush() override;

    protected:
        void notifyPlatformRun_();
        void notifyPlatformStop_();
        void notifyApplicationBeginUpdate_();
        void notifyApplicationEndUpdate_();
        void notifyBootstrapperInitializeBaseServices_();
        void notifyBootstrapperCreateApplication_();

    protected:
        AndroidEventationHubPtr m_androidEventationHub;

        typedef Pool<ConstStringHolderJString, 256> PoolConstStringHolderJString;
        PoolConstStringHolderJString m_poolJString;

        typedef IntrusiveList<ConstStringHolderJString> IntrusiveListConstStringHolderJString;
        IntrusiveListConstStringHolderJString m_holdersJString;

        LoggerInterfacePtr m_proxyLogger;
    };
}
