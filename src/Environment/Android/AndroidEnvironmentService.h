#pragma once

#include "Interface/AndroidEnvironmentServiceInterface.h"
#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/LoggerInterface.h"

#include "Kernel/ServiceBase.h"

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
        jclass getJClassMengineActivity() const override;
        jobject getJObjectMengineActivity() const override;

    public:
        jclass getJClassObject() const override;
        jclass getJClassBoolean() const override;
        jclass getJClassCharacter() const override;
        jclass getJClassInteger() const override;
        jclass getJClassLong() const override;
        jclass getJClassFloat() const override;
        jclass getJClassDouble() const override;
        jclass getJClassString() const override;
        jclass getJClassArrayList() const override;
        jclass getJClassMap() const override;
        jclass getJClassHashMap() const override;

    public:
        size_t getAndroidId( Char * _androidId, size_t _capacity ) const override;
        size_t getDeviceName( Char * _deviceName, size_t _capacity ) const override;
        size_t getDeviceLanguage( Char * _deviceName, size_t _capacity ) const override;
        size_t getAndroidPackageName( Char * _packageName, size_t _capacity ) const override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body ) override;

    public:
        int32_t androidOpenAssetFile( const Char * _path ) override;
        int32_t androidAvailableAssetFile( int32_t _fileId ) override;
        int32_t androidReadAssetFile( int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer ) override;
        int32_t androidSkipAssetFile( int32_t _fileId, int32_t _offset ) override;
        void androidResetAssetFile( int32_t _fileId ) override;
        void androidCloseAssetFile( int32_t _fileId ) override;

    protected:
        void addAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;
        void removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;

    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;

    protected:
        void notifyApplicationRun_();
        void notifyApplicationReady_();
        void notifyApplicationStop_();
        void notifyBootstrapperInitializeBaseServices_();
        void notifyBootstrapperCreateApplication_();

    protected:
        jobject makeJObjectBoolean( JNIEnv * _jenv, bool _value );
        jobject makeJObjectInteger( JNIEnv * _jenv, int32_t _value );
        jobject makeJObjectLong( JNIEnv * _jenv, int64_t _value );
        jobject makeJObjectFloat( JNIEnv * _jenv, float _value );
        jobject makeJObjectDouble( JNIEnv * _jenv, double _value );
        jobject makeJObjectString( JNIEnv * _jenv, const Char * _value );
        jobject makeJObjectHashMap( JNIEnv * _jenv, int32_t _count );

    protected:
        jclass m_jclass_Object;
        jclass m_jclass_Boolean;
        jclass m_jclass_Character;
        jclass m_jclass_Integer;
        jclass m_jclass_Long;
        jclass m_jclass_Float;
        jclass m_jclass_Double;
        jclass m_jclass_String;
        jclass m_jclass_ArrayList;
        jclass m_jclass_Map;
        jclass m_jclass_HashMap;

        AndroidEventationHubPtr m_androidEventationHub;

        LoggerInterfacePtr m_proxyLogger;
    };
}
