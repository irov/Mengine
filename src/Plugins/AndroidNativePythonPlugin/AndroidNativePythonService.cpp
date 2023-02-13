#include "AndroidNativePythonService.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
static Mengine::AndroidNativePythonService * s_androidNativePythonService = nullptr;
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1call )(JNIEnv * env, jclass cls, jstring _plugin, jstring _method, int _id, jobjectArray _args)
    {
        const Mengine::Char * plugin_str = env->GetStringUTFChars( _plugin, nullptr );
        const Mengine::Char * method_str = env->GetStringUTFChars( _method, nullptr );

        Mengine::String plugin = plugin_str;
        Mengine::String method = method_str;

        env->ReleaseStringUTFChars( _plugin, plugin_str );
        env->ReleaseStringUTFChars( _method, method_str );

        if( s_androidNativePythonService == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android call plugin '%s' method '%s'"
                , plugin.c_str()
                , method.c_str()
            );

            return;
        }

        jobjectArray new_args = (jobjectArray)env->NewGlobalRef( _args );

        s_androidNativePythonService->addCommand([plugin, method, _id, new_args]( const Mengine::AndroidNativePythonEventHandlerInterfacePtr & _handler )
        {
            _handler->pythonMethod( plugin, method, _id, new_args );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1addPlugin )(JNIEnv * env, jclass cls, jstring _name, jobject _plugin)
    {
        const Mengine::Char * name_str = env->GetStringUTFChars( _name, nullptr );

        Mengine::String name = name_str;

        env->ReleaseStringUTFChars( _name, name_str );

        if( s_androidNativePythonService == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android add plugin '%s'"
                , name.c_str()
            );

            return;
        }

        jobject new_plugin = env->NewGlobalRef( _plugin );

        s_androidNativePythonService->addCommand([name, new_plugin]( const Mengine::AndroidNativePythonEventHandlerInterfacePtr & _handler )
        {
            _handler->addPlugin( name, new_plugin );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1activateSemaphore )(JNIEnv * env, jclass cls, jstring _name)
    {
        const Mengine::Char * name_str = env->GetStringUTFChars( _name, nullptr );

        Mengine::String name = name_str;

        env->ReleaseStringUTFChars( _name, name_str );

        if(s_androidNativePythonService == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android activate semaphore '%s'"
                , name.c_str()
            );

            return;
        }

        s_androidNativePythonService->addCommand([name]( const Mengine::AndroidNativePythonEventHandlerInterfacePtr & _handler )
        {
            _handler->activateSemaphore( name );
        } );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AndroidNativePythonService, Mengine::AndroidNativePythonService );
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonService::AndroidNativePythonService()
        : m_kernel( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonService::~AndroidNativePythonService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::_initializeService()
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
            ->getKernel();

        pybind::def_functor( kernel, "setAndroidCallback", this, &AndroidNativePythonService::setAndroidCallback );
        pybind::def_functor_args( kernel, "androidMethod", this, &AndroidNativePythonService::androidMethod );
        pybind::def_functor_args( kernel, "androidBooleanMethod", this, &AndroidNativePythonService::androidBooleanMethod );
        pybind::def_functor_args( kernel, "androidInteger32Method", this, &AndroidNativePythonService::androidInteger32Method );
        pybind::def_functor_args( kernel, "androidInteger64Method", this, &AndroidNativePythonService::androidInteger64Method );
        pybind::def_functor_args( kernel, "androidFloatMethod", this, &AndroidNativePythonService::androidFloatMethod );
        pybind::def_functor_args( kernel, "androidStringMethod", this, &AndroidNativePythonService::androidStringMethod );
        pybind::def_functor_args( kernel, "androidConfigMethod", this, &AndroidNativePythonService::androidConfigMethod );

        pybind::def_functor_args( kernel, "waitAndroidSemaphore", this, &AndroidNativePythonService::waitAndroidSemaphore );

        m_kernel = kernel;

        m_eventation = Helper::makeFactorableUnique<PythonEventation>( MENGINE_DOCUMENT_FACTORABLE );

        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation->initialize( mutex ) == false )
        {
            return false;
        }

        s_androidNativePythonService = this;

        m_eventation->setEventHandler( AndroidNativePythonEventHandlerInterfacePtr::from( this ) );

        ANDROID_ENVIRONMENT_SERVICE()
            ->addAndroidEventation( m_eventation );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_initializePlugins = ANDROID_ENVIRONMENT_SERVICE()
            ->getMengineActivityMethodID( jenv, "pythonInitializePlugins", "()V" );

        ANDROID_ENVIRONMENT_SERVICE()
            ->callVoidMengineActivityMethod( jenv, jmethodID_initializePlugins );

        m_eventation->invoke();

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE, &AndroidNativePythonService::notifyApplicationBeginUpdate_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_END_UPDATE, &AndroidNativePythonService::notifyApplicationEndUpdate_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_END_UPDATE );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_pythonFinalizePlugins = ANDROID_ENVIRONMENT_SERVICE()
            ->getMengineActivityMethodID( jenv, "pythonFinalizePlugins", "()V" );

        ANDROID_ENVIRONMENT_SERVICE()
            ->callVoidMengineActivityMethod( jenv, jmethodID_pythonFinalizePlugins );

        s_androidNativePythonService = nullptr;

        m_semaphoreListeners.clear();
        m_callbacks.clear();

        for( auto && [name, jplugin] : m_plugins )
        {
            jenv->DeleteGlobalRef( jplugin );
        }

        m_plugins.clear();

        ANDROID_ENVIRONMENT_SERVICE()
            ->removeAndroidEventation( m_eventation );

        m_eventation->finalize();
        m_eventation = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::addCommand( const LambdaPythonEventHandler & _command )
    {
        m_eventation->addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::notifyApplicationBeginUpdate_()
    {
        m_eventation->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::notifyApplicationEndUpdate_()
    {
        m_eventation->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonService::getPythonAttribute( JNIEnv * _jenv, jobject _obj )
    {
        PyObject * py_value = nullptr;

        if( _obj == nullptr )
        {
            py_value = m_kernel->ret_none();
        }
        else if( _jenv->IsInstanceOf(_obj, Helper::JClassDefinition::BOOLEAN ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( Helper::JClassDefinition::BOOLEAN, "booleanValue", "()Z" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Boolean [booleanValue] ()C'" );

            jboolean value = _jenv->CallBooleanMethod( _obj, methodValue );

            py_value = m_kernel->ret_bool( value );
        }
        else if( _jenv->IsInstanceOf(_obj, Helper::JClassDefinition::CHARACTER ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( Helper::JClassDefinition::CHARACTER, "charValue", "()C" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Character [charValue] ()C'" );

            jchar value = _jenv->CallCharMethod( _obj, methodValue );

            Char value_str[2] = {(Char)value, '\0'};
            py_value = m_kernel->string_from_char_size( value_str, 1 );
        }
        else if( _jenv->IsInstanceOf(_obj, Helper::JClassDefinition::INTEGER ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( Helper::JClassDefinition::INTEGER, "intValue", "()I" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Integer [intValue] ()I'" );

            jint value = _jenv->CallIntMethod( _obj, methodValue );

            py_value = m_kernel->ptr_int32( value );
        }
        else if( _jenv->IsInstanceOf( _obj, Helper::JClassDefinition::LONG ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( Helper::JClassDefinition::LONG, "longValue", "()J" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Long [longValue] ()J'" );

            jlong value = _jenv->CallLongMethod(_obj, methodValue );

            py_value = m_kernel->ptr_int64( value );
        }
        else if( _jenv->IsInstanceOf( _obj, Helper::JClassDefinition::FLOAT ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( Helper::JClassDefinition::FLOAT, "floatValue", "()F" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Float [floatValue] ()F'" );

            jfloat value = _jenv->CallFloatMethod( _obj, methodValue );

            py_value = m_kernel->ptr_float( value );
        }
        else if(_jenv->IsInstanceOf( _obj, Helper::JClassDefinition::DOUBLE ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( Helper::JClassDefinition::DOUBLE, "doubleValue", "()D" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Double [doubleValue] ()D'" );

            jfloat value = _jenv->CallDoubleMethod( _obj, methodValue );

            py_value = m_kernel->ptr_float( value );
        }
        else if( _jenv->IsInstanceOf(_obj, Helper::JClassDefinition::STRING ) == JNI_TRUE )
        {
            const Char * obj_str = _jenv->GetStringUTFChars( (jstring)_obj, nullptr );

            py_value = m_kernel->string_from_char( obj_str );

            _jenv->ReleaseStringUTFChars( (jstring)_obj, obj_str );
        }
        else if( _jenv->IsInstanceOf( _obj, Helper::JClassDefinition::ARRAY_LIST ) == JNI_TRUE )
        {
            static jfieldID fieldElementData = _jenv->GetFieldID( Helper::JClassDefinition::ARRAY_LIST, "elementData", "[Ljava/lang/Object;" );

            MENGINE_ASSERTION_FATAL( fieldElementData != nullptr );

            jobjectArray list_elementData = (jobjectArray)_jenv->GetObjectField( _obj, fieldElementData );

            jsize list_size = _jenv->GetArrayLength( list_elementData );

            PyObject * py_list = m_kernel->tuple_new( list_size );

            for( jsize index = 0; index != list_size; ++index )
            {
                jobject list_obj = _jenv->GetObjectArrayElement( list_elementData, index );

                PyObject * py_obj = this->getPythonAttribute( _jenv, list_obj );

                MENGINE_ASSERTION_FATAL( py_obj != nullptr );

                m_kernel->tuple_setitem( py_list, index, py_obj );

                _jenv->DeleteLocalRef( list_obj );
            }

            _jenv->DeleteLocalRef( list_elementData );

            py_value = py_list;
        }
        else
        {
            jclass cls_obj = _jenv->GetObjectClass( _obj );

            jmethodID jmethodID_getClass = _jenv->GetMethodID( cls_obj, "getClass", "()Ljava/lang/Class;" );

            MENGINE_ASSERTION_FATAL( jmethodID_getClass != nullptr, "invalid get android method 'getClass()Ljava/lang/Class;'" );

            jobject obj_class = _jenv->CallObjectMethod( _obj, jmethodID_getClass );

            jclass cls_class = _jenv->GetObjectClass( obj_class );

            jmethodID jmethodID_getName = _jenv->GetMethodID( cls_class, "getName", "()Ljava/lang/String;" );

            MENGINE_ASSERTION_FATAL( jmethodID_getName != nullptr, "invalid get android method 'getName()Ljava/lang/String;'" );

            jstring obj_class_name = (jstring)_jenv->CallObjectMethod( obj_class, jmethodID_getName );

            const Char * obj_class_name_str = _jenv->GetStringUTFChars( obj_class_name, nullptr );

            LOGGER_ERROR( "unsupported java argument type '%s'"
                , obj_class_name_str
            );

            _jenv->ReleaseStringUTFChars( obj_class_name, obj_class_name_str );
            _jenv->DeleteLocalRef( obj_class_name );

            _jenv->DeleteLocalRef( obj_class );
            _jenv->DeleteLocalRef( cls_class );
            _jenv->DeleteLocalRef( cls_obj );
        }

        return py_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::pythonMethod( const String & _plugin, const String & _method, int32_t _id, jobjectArray _args )
    {
        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        ConstString plugin_c = Helper::stringizeString( _plugin );
        ConstString method_c = Helper::stringizeString( _method );

        LOGGER_INFO( "android", "call python plugin '%s' method '%s' [%s]"
            , _plugin.c_str()
            , _method.c_str()
            , (m_callbacks.find( Helper::makePair( plugin_c, method_c ) ) != m_callbacks.end() ? "Found" : "NOT-FOUND")
        );

        MapAndroidCallbacks::const_iterator it_found = m_callbacks.find( Helper::makePair( plugin_c, method_c ) );

        if( it_found == m_callbacks.end() )
        {
            jenv->DeleteGlobalRef( _args );

            return;
        }

        jsize args_size = jenv->GetArrayLength( _args );

        PyObject * py_args = m_kernel->tuple_new( args_size );

        for( jsize index = 0; index != args_size; ++index )
        {
            jobject obj = jenv->GetObjectArrayElement( _args, index );

            PyObject * py_arg = this->getPythonAttribute( jenv, obj );

            MENGINE_ASSERTION_FATAL( py_arg != nullptr, "android plugin '%s' method '%s' id '%d' invalid arg"
                , _plugin.c_str()
                , _method.c_str()
                , _id
            );

            m_kernel->tuple_setitem( py_args, index, py_arg );

            jenv->DeleteLocalRef( obj );
        }

        jenv->DeleteGlobalRef( _args );

        const pybind::object & cb = it_found->second;

        pybind::object py_result = cb.call_native( pybind::tuple( m_kernel, py_args, pybind::borrowed ) );

        MENGINE_ASSERTION_FATAL( py_result != nullptr, "android plugin '%s' method '%s' id '%d' invalid call"
            , _plugin.c_str()
            , _method.c_str()
            , _id
        );

        MENGINE_ASSERTION_FATAL( !(py_result.is_none() == true && _id != 0), "android plugin '%s' method '%s' return 'None' but have callback '%d'"
            , _plugin.c_str()
            , _method.c_str()
            , _id
        );

        MENGINE_ASSERTION_FATAL( !(py_result.is_none() == false && _id == 0), "android plugin '%s' method '%s' return '%s' but not setup callback"
            , _plugin.c_str()
            , _method.c_str()
            , py_result.str().c_str()
        );

        if( _id == 0 )
        {
            return;
        }

        this->androidResponse( jenv, _id, py_result );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::addPlugin( const String & _name, jobject _plugin )
    {
        MENGINE_ASSERTION_FATAL( m_plugins.find( Helper::stringizeString( _name ) ) == m_plugins.end(), "invalid add plugin '%s' [double]"
            , _name.c_str()
        );

        SCRIPT_SERVICE()
            ->setAvailablePlugin( _name.c_str(), true );

        m_plugins.emplace( Helper::stringizeString( _name ), _plugin );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::setAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb )
    {
        MENGINE_ASSERTION_FATAL( m_callbacks.find( Helper::makePair( _plugin, _method ) ) == m_callbacks.end(), "invalid add plugin '%s' callback '%s' [double]"
            , _plugin.c_str()
            , _method.c_str()
        );

        m_callbacks.emplace( Helper::makePair( _plugin, _method ), _cb );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::androidResponse( JNIEnv * _jenv, int32_t _id, const pybind::object & _result ) const
    {
        m_eventation->invoke();

        jobject jresult;

        if( _result.is_none() == true )
        {
            static jmethodID constructor = _jenv->GetMethodID( Helper::JClassDefinition::OBJECT, "<init>", "()V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] ()V'" );

            jresult = _jenv->NewObject( Helper::JClassDefinition::OBJECT, constructor );
        }
        else if( _result.is_bool() == true )
        {
            static jmethodID constructor = _jenv->GetMethodID( Helper::JClassDefinition::BOOLEAN, "<init>", "(Z)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] (Z)V'" );

            jresult = _jenv->NewObject( Helper::JClassDefinition::BOOLEAN, constructor, (bool)_result.extract() );
        }
        else if( _result.is_integer() == true )
        {
            static jmethodID constructor = _jenv->GetMethodID( Helper::JClassDefinition::INTEGER, "<init>", "(I)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] (I)V'" );

            jresult = _jenv->NewObject( Helper::JClassDefinition::INTEGER, constructor, (int32_t)_result.extract() );
        }
        else if( _result.is_long() == true )
        {
            static jmethodID constructor = _jenv->GetMethodID( Helper::JClassDefinition::LONG, "<init>", "(L)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] (L)V'" );

            jresult = _jenv->NewObject( Helper::JClassDefinition::LONG, constructor, (int64_t)_result.extract() );
        }
        else if( _result.is_float() == true )
        {
            static jmethodID constructor = _jenv->GetMethodID( Helper::JClassDefinition::FLOAT, "<init>", "(F)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] (F)V'" );

            jresult = _jenv->NewObject( Helper::JClassDefinition::FLOAT, constructor, (float)_result.extract() );
        }
        else if( _result.is_string() == true )
        {
            const Char * value_str = (const Char *)_result.extract();

            jstring jvalue = _jenv->NewStringUTF( value_str );

            static jmethodID constructor = _jenv->GetMethodID( Helper::JClassDefinition::STRING, "<init>", "(Ljava/lang/String;)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] (Ljava/lang/String;)V'" );

            jresult = _jenv->NewObject( Helper::JClassDefinition::STRING, constructor, jvalue );

            _jenv->DeleteLocalRef( jvalue );
        }
        else
        {
            LOGGER_ERROR( "android response call '%d' unsupported result type [%s]"
                , _id
                , _result.repr_type().c_str()
            );

            return false;
        }

        static jmethodID jmethodID_pythonCallResponse = ANDROID_ENVIRONMENT_SERVICE()
            ->getMengineActivityMethodID(_jenv, "pythonCallResponse", "(ILjava/lang/Object;)V" );

        ANDROID_ENVIRONMENT_SERVICE()
            ->callVoidMengineActivityMethod(_jenv, jmethodID_pythonCallResponse, _id, jresult );

        _jenv->DeleteLocalRef( jresult );

        m_eventation->invoke();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [void]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "V", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return;
        }

        jenv->CallVoidMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [boolean]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Z", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return false;
        }

        jboolean jresult = jenv->CallBooleanMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (bool)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidNativePythonService::androidInteger32Method( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [int]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "I", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0;
        }

        jint jresult = jenv->CallBooleanMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (int32_t)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidNativePythonService::androidInteger64Method( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [long]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "J", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0;
        }

        jlong jresult = jenv->CallBooleanMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (int64_t)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidNativePythonService::androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [float]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "F", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0.f;
        }

        jfloat jresult = jenv->CallFloatMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (float)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    String AndroidNativePythonService::androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [string]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Ljava/lang/String;", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return String();
        }

        jstring jresult = (jstring)jenv->CallObjectMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        const Mengine::Char * result_str = jenv->GetStringUTFChars( jresult, nullptr );

        Mengine::String result = result_str;

        jenv->ReleaseStringUTFChars( jresult, result_str );
        jenv->DeleteLocalRef( jresult );

        m_eventation->invoke();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonService::androidConfigMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [config]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Ljava/util/Map;", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return m_kernel->ret_none();
        }

        jobject jresult = jenv->CallObjectMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        PyObject * pyresult = m_kernel->dict_new();

        static jmethodID jmethodID_Map_entrySet = jenv->GetMethodID( Helper::JClassDefinition::MAP, "entrySet", "()Ljava/util/Set;" );
        static jmethodID jmethodID_Set_iterator = jenv->GetMethodID( Helper::JClassDefinition::SET, "iterator", "()Ljava/util/Iterator;" );
        static jmethodID jmethodID_Iterator_hasNext = jenv->GetMethodID( Helper::JClassDefinition::ITERATOR, "hasNext", "()Z" );
        static jmethodID jmethodID_Iterator_next = jenv->GetMethodID( Helper::JClassDefinition::ITERATOR, "next", "()Ljava/lang/Object;" );
        static jmethodID jmethodID_MapEntry_getKey = jenv->GetMethodID( Helper::JClassDefinition::MAP_ENTRY, "getKey", "()Ljava/lang/Object;" );
        static jmethodID jmethodID_MapEntry_getValue = jenv->GetMethodID( Helper::JClassDefinition::MAP_ENTRY, "getValue", "()Ljava/lang/Object;" );

        jobject jset = jenv->CallObjectMethod( jresult, jmethodID_Map_entrySet );
        jobject jset_iterator = jenv->CallObjectMethod( jset, jmethodID_Set_iterator );

        jboolean hasNext = jenv->CallBooleanMethod( jset_iterator, jmethodID_Iterator_hasNext );

        while( hasNext == JNI_TRUE )
        {
            jobject jentry = jenv->CallObjectMethod(jset_iterator, jmethodID_Iterator_next );

            jstring jkey = (jstring)jenv->CallObjectMethod( jentry, jmethodID_MapEntry_getKey );
            jstring jvalue = (jstring)jenv->CallObjectMethod( jentry, jmethodID_MapEntry_getValue );

            const Char * key_str = jenv->GetStringUTFChars(jkey, 0);
            const Char * value_str = jenv->GetStringUTFChars(jvalue, 0);

            PyObject * value_pystr = m_kernel->string_from_char( value_str );
            m_kernel->dict_setstring( pyresult, key_str, value_pystr );

            jenv->ReleaseStringUTFChars( jkey, key_str );
            jenv->ReleaseStringUTFChars( jvalue, value_str );

            jenv->DeleteLocalRef( jentry );

            hasNext = jenv->CallBooleanMethod(jset_iterator, jmethodID_Iterator_hasNext );
        }

        jenv->DeleteLocalRef( jset_iterator );
        jenv->DeleteLocalRef( jset );
        jenv->DeleteLocalRef( jresult );

        m_eventation->invoke();

        return pyresult;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::getAndroidMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jfree, uint32_t * const _freeCount, jobject * const _jplugin, jmethodID * const _jmethodId ) const
    {
        MENGINE_ASSERTION_FATAL( _args.size() <= 32, "android method plugin '%s' method '%s' max args [32 < %u]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.size()
        );

        MapAndroidPlugins::const_iterator it_found = m_plugins.find( _plugin );

        if( it_found == m_plugins.end() )
        {
            LOGGER_ERROR( "android not register plugin '%s' (call method '%s' args '%s')"
                , _plugin.c_str()
                , _method.c_str()
                , m_kernel->object_repr( _args.ptr() ).c_str()
            );

            return false;
        }

        jobject jplugin = it_found->second;

        jclass plugin_class = _jenv->GetObjectClass( jplugin );

        if( jplugin == nullptr )
        {
            if( _jenv->ExceptionCheck() == true )
            {
                _jenv->ExceptionClear();
            }

            LOGGER_ERROR( "android not found java plugin '%s' (call method '%s' args '%s')"
                , _plugin.c_str()
                , _method.c_str()
                , m_kernel->object_repr( _args.ptr() ).c_str()
            );

            return false;
        }

        Char signature[1024] = {'\0'};

        MENGINE_STRCAT( signature, "(" );

        MENGINE_MEMSET( _jargs, 0, sizeof( jvalue ) * 32 );
        MENGINE_MEMSET( _jfree, 0, sizeof( jstring ) * 32 );

        uint32_t index_args = 0;
        uint32_t index_free = 0;

        for( const pybind::object & arg : _args )
        {
            if( arg.is_none() == true )
            {
                _jargs[index_args++].l = nullptr;

                MENGINE_STRCAT( signature, "L" );
            }
            else if( arg.is_bool() == true )
            {
                jboolean jvalue = (bool)arg.extract();
                _jargs[index_args++].z = jvalue;

                MENGINE_STRCAT( signature, "Z" );
            }
            else if( arg.is_integer() == true )
            {
                jint jvalue = (int32_t)arg.extract();
                _jargs[index_args++].i = jvalue;

                MENGINE_STRCAT( signature, "I" );
            }
            else if( arg.is_long() == true )
            {
                jlong jvalue = (int64_t)arg.extract();
                _jargs[index_args++].j = jvalue;

                MENGINE_STRCAT( signature, "J" );
            }
            else if( arg.is_float() == true )
            {
                jfloat jvalue = (float)arg.extract();
                _jargs[index_args++].f = jvalue;

                MENGINE_STRCAT( signature, "F" );
            }
            else if( arg.is_string() == true )
            {
                const Char * value_str = (const Char *)arg.extract();

                jstring jvalue = _jenv->NewStringUTF( value_str );

                _jargs[index_args++].l = jvalue;
                _jfree[index_free++] = jvalue;

                MENGINE_STRCAT( signature, "Ljava/lang/String;" );
            }
            else if( arg.is_list() == true )
            {
                pybind::list l = arg.extract();

                pybind::list::size_type s = l.size();

                static jmethodID jmethodID_List_constructor = _jenv->GetMethodID( Helper::JClassDefinition::ARRAY_LIST, "<init>", "(I)V" );

                MENGINE_ASSERTION_FATAL( jmethodID_List_constructor != nullptr, "invalid get android method 'java/lang/ArrayList [<init>] (I)V'" );

                jobject jlist = _jenv->NewObject( Helper::JClassDefinition::ARRAY_LIST, jmethodID_List_constructor, s);

                uint32_t index = 0;
                for( const pybind::object & o : l )
                {
                    const Char * o_str = o.extract();

                    jstring jelement = _jenv->NewStringUTF( o_str );

                    static jmethodID jmethodID_List_add = _jenv->GetMethodID( Helper::JClassDefinition::ARRAY_LIST, "add", "(Ljava/lang/Object;)Z" );

                    MENGINE_ASSERTION_FATAL( jmethodID_List_add != nullptr, "invalid get android method 'java/lang/ArrayList [add] (Ljava/lang/Object;)Z'" );

                    jboolean result = _jenv->CallBooleanMethod( jlist, jmethodID_List_add, jelement );

                    _jenv->DeleteLocalRef( jelement );

                    ++index;
                }

                _jargs[index_args++].l = jlist;
                _jfree[index_free++] = jlist;

                MENGINE_STRCAT( signature, "Ljava/util/List;" );
            }
            else
            {
                _jenv->DeleteLocalRef( plugin_class );

                LOGGER_ERROR( "android plugin '%s' method '%s' unsupported arg [%u] type [%s]"
                    , _plugin.c_str()
                    , _method.c_str()
                    , index_args
                    , m_kernel->object_repr_type( arg.ptr() ).c_str()
                );

                return false;
            }
        }

        MENGINE_STRCAT( signature, ")" );
        MENGINE_STRCAT( signature, _retType );

        jmethodID jmethodId = _jenv->GetMethodID( plugin_class, _method.c_str(), signature );

        if( jmethodId == nullptr )
        {
            if( _jenv->ExceptionCheck() == true )
            {
                _jenv->ExceptionClear();
            }

            _jenv->DeleteLocalRef( plugin_class );

            LOGGER_ERROR( "android plugin '%s' not found method '%s' with signature '%s'"
                , _plugin.c_str()
                , _method.c_str()
                , signature
            );

            return false;
        }

        _jenv->DeleteLocalRef( plugin_class );

        *_freeCount = index_free;
        *_jplugin = jplugin;
        *_jmethodId = jmethodId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::waitAndroidSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
    {
        AndroidSemaphoreListenerDesc desc;
        desc.name = _name;
        desc.cb = _cb;
        desc.args = _args;

        m_semaphoreListeners.emplace_back( desc );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jmethodID jmethodID_waitAndroidSemaphore = ANDROID_ENVIRONMENT_SERVICE()
            ->getMengineActivityMethodID( jenv, "waitAndroidSemaphore", "(Ljava/lang/String;)V" );

        const Char * name_str = _name.c_str();

        jstring name_jvalue = jenv->NewStringUTF( name_str );

        ANDROID_ENVIRONMENT_SERVICE()
            ->callVoidMengineActivityMethod( jenv, jmethodID_waitAndroidSemaphore, name_jvalue );

        jenv->DeleteLocalRef( name_jvalue );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::activateSemaphore( const String & _name )
    {
        for( const AndroidSemaphoreListenerDesc & desc : m_semaphoreListeners )
        {
            if( desc.name != _name.c_str() )
            {
                continue;
            }

            desc.cb.call_args( desc.args );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
