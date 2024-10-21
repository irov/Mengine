#pragma once

#include "Interface/AndroidKernelServiceInterface.h"
#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/LoggerInterface.h"

#include "ConstStringHolderJString.h"
#include "AndroidAnalyticsEventProvider.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Pool.h"
#include "Kernel/IntrusiveList.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class AndroidKernelService
        : public ServiceBase<AndroidKernelServiceInterface>
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
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body ) override;
        bool openDeleteAccount() override;

    protected:
        void notifyPlatformRun_();
        void notifyPlatformStop_();
        void notifyBootstrapperInitializeBaseServices_();
        void notifyBootstrapperCreateApplication_();

    protected:
        AndroidAnalyticsEventProviderPtr m_androidAnalyticsEventProvider;

        ThreadMutexInterfacePtr m_mutexJStrings;

        typedef Pool<ConstStringHolderJString, 256> PoolConstStringHolderJString;
        PoolConstStringHolderJString m_poolJString;

        typedef IntrusiveList<ConstStringHolderJString> IntrusiveListConstStringHolderJString;
        IntrusiveListConstStringHolderJString m_holdersJString;

        LoggerInterfacePtr m_proxyLogger;
    };
}
