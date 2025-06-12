#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Environment/Android/AndroidKernelServiceInterface.h"

#include "ConstStringHolderJString.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Pool.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"
#include "Kernel/IntrusiveList.h"

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
        void stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr ) override;

    public:
        void addPlugin( const ConstString & _plugin, jobject _jmodule ) override;
        jobject getPlugin( const ConstString & _plugin ) override;
        jobject removePlugin( const ConstString & _plugin ) override;

    public:
        bool hasPluginMethod( const ConstString & _plugin, const ConstString & _method ) const override;
        void callPluginMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, jobjectArray _args ) const override;

    public:
        AndroidPluginCallbackInterfacePtr addPluginCallback( const ConstString & _plugin, const ConstString & _method, const AndroidPluginCallbackInterfacePtr & _callback ) override;
        void removePluginCallback( const ConstString & _plugin, const ConstString & _method, const AndroidPluginCallbackInterfacePtr & _callback ) override;

    public:
        void activateSemaphore( const ConstString & _semaphore ) override;
        AndroidSemaphoreListenerInterfacePtr waitSemaphore( const ConstString & _semaphore, const AndroidSemaphoreListenerInterfacePtr & _listener ) override;

    protected:
        ThreadMutexInterfacePtr m_mutexJStrings;

        typedef Pool<ConstStringHolderJString, 256> PoolConstStringHolderJString;
        PoolConstStringHolderJString m_poolJString;

        typedef IntrusiveList<ConstStringHolderJString> IntrusiveListConstStringHolderJString;
        IntrusiveListConstStringHolderJString m_holdersJString;

        ThreadMutexInterfacePtr m_semaphoresMutex;
        ThreadMutexInterfacePtr m_callbacksMutex;
        ThreadMutexInterfacePtr m_pluginsMutex;

        typedef Vector<AndroidPluginCallbackInterfacePtr> VectorAndroidCallbacks;

        typedef Map<Pair<ConstString, ConstString>, VectorAndroidCallbacks> MapAndroidCallbacks;
        MapAndroidCallbacks m_callbacks;

        typedef Vector<AndroidSemaphoreListenerInterfacePtr> VectorAndroidSemaphoreListeners;

        struct SemaphoreDesc
        {
            bool activated;
            VectorAndroidSemaphoreListeners listeners;
        };

        typedef Map<ConstString, SemaphoreDesc> MapAndroidSemaphores;
        MapAndroidSemaphores m_semaphores;

        typedef Map<ConstString, jobject> MapAndroidPlugins;
        MapAndroidPlugins m_plugins;
    };
}