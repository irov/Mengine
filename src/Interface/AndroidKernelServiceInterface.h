#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidEventationHub.h"

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
        virtual void addAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) = 0;
        virtual void removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) = 0;
        virtual void invokeAndroidEventations() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANDROID_KERNEL_SERVICE()\
    ((Mengine::AndroidKernelServiceInterface *)SERVICE_GET(Mengine::AndroidKernelServiceInterface))
//////////////////////////////////////////////////////////////////////////