#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidEventationHub.h"

#include "Config/Char.h"

namespace Mengine
{
    class AndroidEnvironmentServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AndroidEnvironmentService" )

    public:
        virtual jclass getJClass( JNIEnv * _jenv, const Char * _signature ) const = 0;

    public:
        virtual jmethodID getMengineActivityMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature ) const = 0;
        virtual void callVoidMengineActivityMethod( JNIEnv * _jenv, jmethodID _method, ... ) const = 0;
        virtual jobject callObjectMengineActivityMethod( JNIEnv * _jenv, jmethodID _method, ... ) const = 0;

    public:
        virtual size_t getAndroidId( Char * _androidId, size_t _capacity ) const = 0;
        virtual size_t getDeviceName( Char * _androidId, size_t _capacity ) const = 0;
        virtual size_t getDeviceLanguage( Char * _deviceName, size_t _capacity ) const = 0;
        virtual size_t getAndroidPackageName( Char * _androidId, size_t _capacity ) const = 0;

    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;
        virtual bool openMail( const Char * _email, const Char * _subject, const Char * _body ) = 0;

    public:
        virtual int32_t androidOpenAssetFile( const Char * _path ) = 0;
        virtual int32_t androidAvailableAssetFile( int32_t _fileId ) = 0;
        virtual int32_t androidReadAssetFile( int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer ) = 0;
        virtual int32_t androidSkipAssetFile( int32_t _fileId, int32_t _offset ) = 0;
        virtual void androidResetAssetFile( int32_t _fileId ) = 0;
        virtual void androidCloseAssetFile( int32_t _fileId ) = 0;

    public:
        virtual void addAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) = 0;
        virtual void removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) = 0;
        virtual void invokeAndroidEventations() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANDROID_ENVIRONMENT_SERVICE()\
    ((Mengine::AndroidEnvironmentServiceInterface *)SERVICE_GET(Mengine::AndroidEnvironmentServiceInterface))
//////////////////////////////////////////////////////////////////////////