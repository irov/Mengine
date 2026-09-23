#include "AndroidKernelService.h"

#include "Interface/PluginServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidDeclaration.h"
#include "Environment/Android/AndroidApplicationHelper.h"

#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidKernelService_1call )(JNIEnv * env, jclass cls, jstring _plugin, jstring _method, jobjectArray _args)
    {
        Mengine::ConstString plugin = Mengine::Helper::AndroidMakeConstStringFromJString( env, _plugin );
        Mengine::ConstString method = Mengine::Helper::AndroidMakeConstStringFromJString( env, _method );

        if( ANDROID_KERNEL_SERVICE()
            ->hasPluginMethod( plugin, method ) == false )
        {
            LOGGER_ERROR( "plugin '%s' method '%s' not found"
                , plugin.c_str()
                , method.c_str()
            );

            return;
        }

        JNIEnv * main_jenv = Mengine::Mengine_JNI_GetEnv();

        ANDROID_KERNEL_SERVICE()
            ->callPluginMethod( main_jenv, plugin, method, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidKernelService_1addPlugin )(JNIEnv * env, jclass cls, jstring _plugin, jobject _jmodule)
    {
        Mengine::ConstString plugin = Mengine::Helper::AndroidMakeConstStringFromJString( env, _plugin );

        jobject new_jmodule = Mengine::Mengine_JNI_NewGlobalRef( env, _jmodule );

        ANDROID_KERNEL_SERVICE()
            ->addPlugin( plugin, new_jmodule );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidKernelService_1removePlugin )(JNIEnv * env, jclass cls, jstring _name)
    {
        Mengine::ConstString plugin = Mengine::Helper::AndroidMakeConstStringFromJString( env, _name );

        jobject jmodule = ANDROID_KERNEL_SERVICE()
            ->removePlugin( plugin );

        Mengine::Mengine_JNI_DeleteGlobalRef( env, jmodule );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidKernelService_1activateSemaphore )(JNIEnv * env, jclass cls, jstring _name, jobject _value)
    {
        Mengine::ConstString semaphore = Mengine::Helper::AndroidMakeConstStringFromJString( env, _name );

        ANDROID_KERNEL_SERVICE()
            ->activateSemaphore( env, semaphore, _value );
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AndroidKernelService, Mengine::AndroidKernelService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidKernelService::AndroidKernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidKernelService::~AndroidKernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidKernelService::_initializeService()
    {
        m_semaphoresMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );
        m_callbacksMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );
        m_pluginsMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::_finalizeService()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        this->clearCallbacks();

        m_semaphoresMutex = nullptr;
        m_callbacksMutex = nullptr;
        m_pluginsMutex = nullptr;

        for( auto && [name, jplugin] : m_plugins )
        {
            Mengine_JNI_DeleteGlobalRef( jenv, jplugin );
        }

        m_plugins.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::addPlugin( const ConstString & _pluginName, jobject _jmodule )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_pluginsMutex );

        MENGINE_ASSERTION_FATAL( m_plugins.find( _pluginName ) == m_plugins.end(), "invalid add plugin '%s' [double]"
            , _pluginName.c_str()
        );

        m_plugins.emplace( _pluginName, _jmodule );

        PLUGIN_SERVICE()
            ->setAvailablePlugin( _pluginName, true );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidKernelService::getPlugin( const ConstString & _plugin )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_pluginsMutex );

        MapAndroidPlugins::const_iterator it_found = m_plugins.find( _plugin );

        if( it_found == m_plugins.end() )
        {
            return nullptr;
        }

        jobject jmodule = it_found->second;

        return jmodule;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidKernelService::removePlugin( const ConstString & _pluginName )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_pluginsMutex );

        MapAndroidPlugins::iterator it_found = m_plugins.find( _pluginName );

        MENGINE_ASSERTION_FATAL( it_found != m_plugins.end(), "invalid remove plugin '%s' [empty]"
            , _pluginName.c_str()
        );

        jobject jmodule = it_found->second;

        m_plugins.erase( it_found );

        PLUGIN_SERVICE()
            ->setAvailablePlugin( _pluginName, true );

        return jmodule;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidKernelService::hasPluginMethod( const ConstString & _plugin, const ConstString & _method ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_callbacksMutex );

        MapAndroidCallbacks::const_iterator it_found = m_callbacks.find( StdUtility::make_pair( _plugin, _method ) );

        if( it_found == m_callbacks.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::callPluginMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, jobjectArray _args ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_callbacksMutex );

        MapAndroidCallbacks::const_iterator it_found = m_callbacks.find( StdUtility::make_pair( _plugin, _method ) );

        if( it_found == m_callbacks.end() )
        {
            return;
        }

        const VectorAndroidCallbacks & callbacks = it_found->second;

        for( const AndroidPluginCallbackInterfacePtr & callback : callbacks )
        {
            callback->invoke( _jenv, _args );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidPluginCallbackInterfacePtr AndroidKernelService::addPluginCallback( const ConstString & _plugin, const ConstString & _method, const AndroidPluginCallbackInterfacePtr & _callback )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_callbacksMutex );

        MapAndroidCallbacks::iterator it_found = m_callbacks.find( StdUtility::make_pair( _plugin, _method ) );

        if( it_found == m_callbacks.end() )
        {
            VectorAndroidCallbacks new_callbacks;

            it_found = m_callbacks.emplace( StdUtility::make_pair( StdUtility::make_pair( _plugin, _method ), new_callbacks ) ).first;
        }

        VectorAndroidCallbacks & callbacks = it_found->second;

        callbacks.emplace_back( _callback );

        return _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::removePluginCallback( const ConstString & _plugin, const ConstString & _method, const AndroidPluginCallbackInterfacePtr & _callback )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_callbacksMutex );

        MapAndroidCallbacks::iterator it_found = m_callbacks.find( StdUtility::make_pair( _plugin, _method ) );

        if( it_found == m_callbacks.end() )
        {
            LOGGER_ERROR("invalid remove android callback plugin '%s' method '%s' not found"
                , _plugin.c_str()
                , _method.c_str()
            );

            return;
        }

        VectorAndroidCallbacks & callbacks = it_found->second;

        VectorAndroidCallbacks::iterator it_callback_found = StdAlgorithm::find_if( callbacks.begin(), callbacks.end(), [_callback](const AndroidPluginCallbackInterfacePtr & callback) {
            return callback == _callback;
        } );

        if( it_callback_found == callbacks.end() )
        {
            LOGGER_ERROR("invalid remove android callback plugin '%s' method '%s' not found [cb]"
                , _plugin.c_str()
                , _method.c_str()
            );

            return;
        }

        callbacks.erase( it_callback_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::activateSemaphore( JNIEnv * _jenv, const ConstString & _semaphore, jobject _value )
    {
        LOGGER_INFO( "android", "activate semaphore '%s'"
            , _semaphore.c_str()
        );

        VectorAndroidSemaphoreListeners listeners;

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_semaphoresMutex );

            MapAndroidSemaphores::iterator it_found = m_semaphores.find( _semaphore );

            if( it_found == m_semaphores.end() )
            {
                return;
            }

            listeners.swap( it_found->second );
            m_semaphores.erase( it_found );
        }

        for( const AndroidSemaphoreListenerInterfacePtr & listener : listeners )
        {
            listener->invoke( _jenv, _value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSemaphoreListenerInterfacePtr AndroidKernelService::waitSemaphore( const ConstString & _semaphore, const AndroidSemaphoreListenerInterfacePtr & _listener )
    {
        LOGGER_INFO( "android", "wait semaphore '%s'"
            , _semaphore.c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jobject semaphore;

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_semaphoresMutex );

            jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, _semaphore );

            semaphore = Helper::AndroidCallObjectApplicationMethod( jenv, "waitSemaphore", "(Ljava/lang/String;)Lorg/Mengine/Base/MengineSemaphore;", jstring_name );

            Mengine_JNI_DeleteLocalRef( jenv, jstring_name );

            if( semaphore == nullptr )
            {
                m_semaphores[_semaphore].emplace_back( _listener );

                return _listener;
            }
        }

        jclass semaphoreClass = Mengine_JNI_GetObjectClass( jenv, semaphore );
        jmethodID getValueMethod = Mengine_JNI_GetMethodID( jenv, semaphoreClass, "getValue", "()Ljava/lang/Object;" );
        jobject value = Mengine_JNI_CallObjectMethod( jenv, semaphore, getValueMethod );

        Mengine_JNI_DeleteLocalRef( jenv, semaphoreClass );
        Mengine_JNI_DeleteLocalRef( jenv, semaphore );

        _listener->invoke( jenv, value );
        Mengine_JNI_DeleteLocalRef( jenv, value );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::clearCallbacks()
    {
        m_semaphoresMutex->lock();
        m_semaphores.clear();
        m_semaphoresMutex->unlock();

        m_callbacksMutex->lock();
        m_callbacks.clear();
        m_callbacksMutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::_update()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jobject jobject_MengineApplication = Mengine_JNI_GetObjectApplication( jenv );

        MENGINE_ASSERTION_FATAL( jobject_MengineApplication != nullptr, "invalid get object application" );

        Helper::AndroidCallVoidStaticClassMethod( jenv, "org/Mengine/Base/MengineNativeCallQueue", "processCalls", "(Lorg/Mengine/Base/MengineApplication;)V", jobject_MengineApplication );

        Mengine_JNI_DeleteLocalRef( jenv, jobject_MengineApplication );
    }
    //////////////////////////////////////////////////////////////////////////
}
