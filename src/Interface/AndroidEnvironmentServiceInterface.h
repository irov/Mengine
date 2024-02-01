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
        virtual size_t getAndroidId( Char * _androidId, size_t _capacity ) const = 0;
        virtual size_t getDeviceName( Char * _androidId, size_t _capacity ) const = 0;
        virtual size_t getDeviceLanguage( Char * _deviceName, size_t _capacity ) const = 0;
        virtual size_t getAndroidPackageName( Char * _androidId, size_t _capacity ) const = 0;

    public:
        virtual void stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr ) = 0;

    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;
        virtual bool openMail( const Char * _email, const Char * _subject, const Char * _body ) = 0;

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