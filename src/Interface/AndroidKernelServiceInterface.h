#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"

namespace Mengine
{
    class AndroidKernelServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AndroidKernelService" )

    public:
        virtual size_t getAndroidId( Char * _androidId, size_t _capacity ) const = 0;

    public:
        virtual void stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr ) = 0;

    public:
        typedef Lambda<void()> LambdaCommand;
        virtual void addCommand( const LambdaCommand & _lambda ) = 0;
        virtual void invokeCommands() = 0;

    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;
        virtual bool openMail( const Char * _email, const Char * _subject, const Char * _body ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANDROID_KERNEL_SERVICE()\
    ((Mengine::AndroidKernelServiceInterface *)SERVICE_GET(Mengine::AndroidKernelServiceInterface))
//////////////////////////////////////////////////////////////////////////