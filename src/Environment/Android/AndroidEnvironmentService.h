#pragma once

#include "Interface/AndroidEnvironmentServiceInterface.h"
#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/LoggerInterface.h"

#include "ConstStringHolderJString.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Pool.h"
#include "Kernel/IntrusiveList.h"

namespace Mengine
{
    class AndroidEnvironmentService
        : public ServiceBase<AndroidEnvironmentServiceInterface>
        , public AnalyticsEventProviderInterface
    {
    public:
        AndroidEnvironmentService();
        ~AndroidEnvironmentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void callVoidApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const override;
        jobject callObjectApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const override;

    public:
        void callVoidActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const override;
        jobject callObjectActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const override;
        jboolean callBooleanActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const override;
        jint callIntActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const override;
        jlong callLongActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const override;


    protected:
        jclass getJClass( JNIEnv * _jenv, const Char * _signature ) const;
        jmethodID getActivityMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature ) const;
        jmethodID getApplicationMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature ) const;

    public:
        size_t getAndroidId( Char * _androidId, size_t _capacity ) const override;
        size_t getDeviceName( Char * _deviceName, size_t _capacity ) const override;
        size_t getDeviceLanguage( Char * _deviceName, size_t _capacity ) const override;
        size_t getAndroidPackageName( Char * _packageName, size_t _capacity ) const override;

    public:
        void stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr ) override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body ) override;

    protected:
        void addAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;
        void removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;
        void invokeAndroidEventations() override;

    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
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
