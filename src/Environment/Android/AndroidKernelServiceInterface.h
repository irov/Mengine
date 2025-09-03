#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"

#include "Environment/Android/AndroidPluginCallbackInterface.h"
#include "Environment/Android/AndroidSemaphoreListenerInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class AndroidKernelServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AndroidKernelService" )

    public:
        virtual void stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr ) = 0;

    public:
        virtual void addPlugin( const ConstString & _plugin, jobject _jmodule ) = 0;
        virtual jobject getPlugin( const ConstString & _plugin ) = 0;
        virtual jobject removePlugin( const ConstString & _plugin ) = 0;

    public:
        virtual bool hasPluginMethod( const ConstString & _plugin, const ConstString & _method ) const = 0;
        virtual void callPluginMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, jobjectArray _args ) const = 0;

    public:
        virtual AndroidPluginCallbackInterfacePtr addPluginCallback( const ConstString & _plugin, const ConstString & _method, const AndroidPluginCallbackInterfacePtr & _callback ) = 0;
        virtual void removePluginCallback( const ConstString & _plugin, const ConstString & _method, const AndroidPluginCallbackInterfacePtr & _callback ) = 0;

    public:
        virtual void activateSemaphore( const ConstString & _semaphore ) = 0;
        virtual AndroidSemaphoreListenerInterfacePtr waitSemaphore( const ConstString & _semaphore, const AndroidSemaphoreListenerInterfacePtr & _listener ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANDROID_KERNEL_SERVICE()\
    ((Mengine::AndroidKernelServiceInterface *)SERVICE_GET(Mengine::AndroidKernelServiceInterface))
//////////////////////////////////////////////////////////////////////////