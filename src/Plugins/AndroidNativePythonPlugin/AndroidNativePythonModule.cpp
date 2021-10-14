#include "AndroidNativePythonModule.h"

#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/AndroidPlatformExtensionInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"

#include "Environment/Android/AndroidUtils.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
static Mengine::AndroidNativePythonModule * s_androidNativePythonModule = nullptr;
//////////////////////////////////////////////////////////////////////////
static jmethodID jmethodID_initializePlugin;
//////////////////////////////////////////////////////////////////////////
extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1setupPythonJNI )(JNIEnv * env, jclass cls)
    {
        jmethodID_initializePlugin = env->GetStaticMethodID( cls, "pythonInitializePlugin", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
    MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1call )(JNIEnv * env, jclass cls, jstring _method, jstring _kwds)
    {
        if( s_androidNativePythonModule != nullptr )
        {
            const char * method_str = env->GetStringUTFChars( _method, nullptr );
            const char * kwds_str = env->GetStringUTFChars( _kwds, nullptr );

            Mengine::String method = method_str;
            Mengine::String kwds = kwds_str;

            env->ReleaseStringUTFChars( _method, method_str );
            env->ReleaseStringUTFChars( _kwds, kwds_str );

            s_androidNativePythonModule->addCommand( [method, kwds]( const Mengine::PythonEventHandlerPtr & _handler )
                                                       {
                                                           _handler->pythonMethod( method, kwds );
                                                       } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
    MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1addPlugin )(JNIEnv * env, jclass cls, jstring _name, jobject _plugin)
    {
        if( s_androidNativePythonModule != nullptr )
        {
            const char * name_str = env->GetStringUTFChars( _name, nullptr );

            Mengine::String name = name_str;

            env->ReleaseStringUTFChars( _name, name_str );

            jobject new_plugin = env->NewGlobalRef(_plugin);

            s_androidNativePythonModule->addCommand( [name, new_plugin]( const Mengine::PythonEventHandlerPtr & _handler )
                                                     {
                                                         _handler->addPlugin( name, new_plugin );
                                                     } );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonModule::AndroidNativePythonModule()
        : m_jenv(nullptr)
        , m_kernel(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonModule::~AndroidNativePythonModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonModule::_initializeModule()
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

        pybind::def_functor( kernel, "setAndroidCallback", this, &AndroidNativePythonModule::setAndroidCallback );
        pybind::def_functor_args( kernel, "androidMethod", this, &AndroidNativePythonModule::androidMethod );

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
                ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation.initialize( mutex ) == false )
        {
            return false;
        }

        m_globals = pybind::make_dict_t( kernel );

        m_kernel = kernel;

        s_androidNativePythonModule = this;

        m_eventation.setEventHandler( PythonEventHandlerPtr::from(this) );

        AndroidPlatformExtensionInterface * extension = PLATFORM_SERVICE()
                ->getDynamicUnknown();

        m_jenv = extension->getJENV();
        m_jclassActivity = extension->getJClassActivity();

        m_jenv->CallStaticVoidMethod( m_jclassActivity, jmethodID_initializePlugin );

        m_eventation.invoke();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::_finalizeModule()
    {
        m_jenv->DeleteGlobalRef( m_jclassActivity );

        m_globals.reset();

        s_androidNativePythonModule = nullptr;

        m_callbacks.clear();

        m_eventation.finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::addCommand( const LambdaPythonEventHandler & _command )
    {
        m_eventation.addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::_update( bool _focus )
    {
        MENGINE_UNUSED(_focus);

        m_eventation.invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::pythonMethod( const String & _method, const String & _args )
    {
        ConstString method_c = Helper::stringizeString( _method );

        MapAndroidCallbacks::iterator it_found = m_callbacks.find( method_c );

        if( it_found == m_callbacks.end() )
        {
            return;
        }

        const Char * method_str = _method.c_str();
        const Char * args_str = _args.c_str();

        PyObject * py_args = m_kernel->eval_string( args_str, m_globals.ptr(), nullptr );

        const pybind::object & cb = it_found->second;

        cb.call_native( pybind::tuple( m_kernel, py_args ) );

        m_kernel->decref( py_args );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::addPlugin( const String & _name, const jobject & _plugin )
    {
        m_plugins.emplace( Helper::stringizeString( _name ), _plugin );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::setAndroidCallback( const ConstString & _method, const pybind::object & _cb )
    {
        m_callbacks.emplace( _method, _cb );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        Char signature[1024] = {'\0'};

        MENGINE_STRCAT(signature, "(");

        jvalue jargs[32];
        MENGINE_MEMSET( jargs, 0, sizeof(jvalue) * 32 );

        jstring jfree[32];

        uint32_t index_args = 0;
        uint32_t index_free = 0;

        for( const pybind::object & arg : _args )
        {
            if( arg.is_none() == true )
            {
                jargs[index_args++].l = nullptr;

                MENGINE_STRCAT(signature, "L");
            }
            else if( arg.is_bool() == true )
            {
                jboolean jvalue = (bool)arg.extract();
                jargs[index_args++].z = jvalue;

                MENGINE_STRCAT(signature, "Z");
            }
            else if( arg.is_integer() == true )
            {
                jint jvalue = (int32_t)arg.extract();
                jargs[index_args++].i = jvalue;

                MENGINE_STRCAT(signature, "I");
            }
            else if( arg.is_long() == true )
            {
                jlong jvalue = (int64_t)arg.extract();
                jargs[index_args++].j = jvalue;

                MENGINE_STRCAT(signature, "J");
            }
            else if( arg.is_float() == true )
            {
                jfloat jvalue = (float)arg.extract();
                jargs[index_args++].f = jvalue;

                MENGINE_STRCAT(signature, "F");
            }
            else if( arg.is_string() == true )
            {
                const Char * value_str = (const Char *)arg.extract();

                jstring jvalue = m_jenv->NewStringUTF( value_str );

                jargs[index_args++].l = jvalue;
                jfree[index_free++] = jvalue;

                MENGINE_STRCAT(signature, "Ljava/lang/String;");
            }
        }

        MENGINE_STRCAT(signature, ")V");

        MapAndroidPlugins::const_iterator it_found = m_plugins.find( _plugin );

        if( it_found == m_plugins.end() )
        {
            return;
        }

        jobject plugin = it_found->second;

        jclass plugin_class = m_jenv->GetObjectClass( plugin );

        jmethodID jmethodID_method = m_jenv->GetMethodID( plugin_class, _method.c_str(), signature );

        if( jmethodID_method == 0 )
        {
            return;
        }

        m_jenv->CallVoidMethodA( plugin, jmethodID_method, jargs );

        for(uint32_t index = 0; index != index_free; ++index )
        {
            jstring j = jfree[index];

            m_jenv->DeleteLocalRef(j);
        }

        m_eventation.invoke();
    }
    //////////////////////////////////////////////////////////////////////////
}
