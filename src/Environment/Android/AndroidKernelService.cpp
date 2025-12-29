#include "AndroidKernelService.h"

#include "Interface/StringizeServiceInterface.h"
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
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidKernelService_1activateSemaphore )(JNIEnv * env, jclass cls, jstring _name)
    {
        Mengine::ConstString semaphore = Mengine::Helper::AndroidMakeConstStringFromJString( env, _name );

        ANDROID_KERNEL_SERVICE()
            ->activateSemaphore( semaphore );
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
        ThreadMutexInterfacePtr mutexJStrings = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        m_mutexJStrings = mutexJStrings;

        m_semaphoresMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );
        m_callbacksMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );
        m_pluginsMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::_finalizeService()
    {
        m_mutexJStrings = nullptr;

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        for( IntrusiveListConstStringHolderJString::iterator
             it = m_holdersJStrings.begin();
             it != m_holdersJStrings.end(); )
        {
            IntrusiveListConstStringHolderJString::iterator it_erase = it;

            ConstStringHolderJString * holder = *it;
            ++it;

            m_holdersJStrings.erase(it_erase );

            holder->removeJString( jenv );

            m_poolJStrings.destroyT(holder );
        }

        m_holdersJStrings.clear();
        m_poolJStrings.clear();

        m_semaphoresMutex = nullptr;
        m_callbacksMutex = nullptr;
        m_pluginsMutex = nullptr;

        m_semaphores.clear();
        m_callbacks.clear();

        for( auto && [name, jplugin] : m_plugins )
        {
            Mengine_JNI_DeleteGlobalRef( jenv, jplugin );
        }

        m_plugins.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr )
    {
        jsize value_length = Mengine_JNI_GetStringLength( _jenv, _value );

        if( value_length == 0 )
        {
            *_cstr = ConstString::none();

            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexJStrings );

        ConstStringHolderJString * holder = m_poolJStrings.createT();

        holder->setJString( _jenv, _value );

        if( STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr ) == false )
        {
            holder->removeJString( _jenv );

            m_poolJStrings.destroyT(holder );

            return;
        }

        m_holdersJStrings.push_back(holder );
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
    void AndroidKernelService::activateSemaphore( const ConstString & _semaphore )
    {
        LOGGER_INFO( "android", "activate semaphore '%s'"
            , _semaphore.c_str()
        );

        MENGINE_THREAD_MUTEX_SCOPE( m_semaphoresMutex );

        MapAndroidSemaphores::iterator it_found = m_semaphores.find( _semaphore );

        if( it_found == m_semaphores.end() )
        {
            SemaphoreDesc desc;
            desc.activated = true;

            m_semaphores.emplace( _semaphore, desc );

            return;
        }

        SemaphoreDesc & semaphore = it_found->second;

        if( semaphore.activated == true )
        {
            return;
        }

        semaphore.activated = true;

        for( const AndroidSemaphoreListenerInterfacePtr & listener : semaphore.listeners )
        {
            listener->invoke();
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

        const Char * name_str = _semaphore.c_str();

        jstring jstring_name = Mengine_JNI_NewStringUTF( jenv, name_str );

        jboolean jresult = Helper::AndroidCallBooleanApplicationMethod( jenv, "waitSemaphore", "(Ljava/lang/String;)Z", jstring_name );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );

        if( jresult == true )
        {
            _listener->invoke();

            return nullptr;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_semaphoresMutex );

        MapAndroidSemaphores::iterator it_found = m_semaphores.find( _semaphore );

        if( it_found == m_semaphores.end() )
        {
            SemaphoreDesc desc;
            desc.activated = false;
            desc.listeners.emplace_back( _listener );

            m_semaphores.emplace( _semaphore, desc );

            return _listener;
        }

        SemaphoreDesc & semaphore = it_found->second;

        if( semaphore.activated == true )
        {
            _listener->invoke();

            return nullptr;
        }

        semaphore.listeners.emplace_back( _listener );

        return _listener;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::_update()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidApplicationMethod( jenv, "processDeferredCalls", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
}