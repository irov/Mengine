#include "AndroidNativePythonModule.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/AndroidPlatformExtensionInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"

#include "Environment/Android/AndroidUtils.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
static Mengine::AndroidNativePythonModule * s_androidNativePythonModule = nullptr;
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

        if( s_androidNativePythonModule == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android call plugin '%s' method '%s'"
                , plugin.c_str()
                , method.c_str()
            );

            return;
        }

        jobjectArray new_args = (jobjectArray)env->NewGlobalRef( _args );

        s_androidNativePythonModule->addCommand( [plugin, method, _id, new_args]( const Mengine::PythonEventHandlerInterfacePtr & _handler )
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

        if( s_androidNativePythonModule == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android add plugin '%s'"
                , name.c_str()
            );

            return;
        }

        jobject new_plugin = env->NewGlobalRef( _plugin );

        s_androidNativePythonModule->addCommand( [name, new_plugin]( const Mengine::PythonEventHandlerInterfacePtr & _handler )
        {
            _handler->addPlugin( name, new_plugin );
        } );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonModule::AndroidNativePythonModule()
        : m_jenv( nullptr )
        , m_jclass_Object( nullptr )
        , m_jclass_Boolean( nullptr )
        , m_jclass_Character( nullptr )
        , m_jclass_Integer( nullptr )
        , m_jclass_Long( nullptr )
        , m_jclass_Float( nullptr )
        , m_jclass_Double( nullptr )
        , m_jclass_String( nullptr )
        , m_jclass_ArrayList( nullptr )
        , m_kernel( nullptr )
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
        pybind::def_functor_args( kernel, "androidBooleanMethod", this, &AndroidNativePythonModule::androidBooleanMethod );
        pybind::def_functor_args( kernel, "androidInteger32Method", this, &AndroidNativePythonModule::androidInteger32Method );
        pybind::def_functor_args( kernel, "androidInteger64Method", this, &AndroidNativePythonModule::androidInteger64Method );
        pybind::def_functor_args( kernel, "androidFloatMethod", this, &AndroidNativePythonModule::androidFloatMethod );
        pybind::def_functor_args( kernel, "androidStringMethod", this, &AndroidNativePythonModule::androidStringMethod );

        m_eventation = Helper::makeFactorableUnique<PythonEventation>( MENGINE_DOCUMENT_FACTORABLE );

        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation->initialize( mutex ) == false )
        {
            return false;
        }

        m_globals = pybind::make_dict_t( kernel );
        m_globals.set( "True", kernel->get_true() );
        m_globals.set( "False", kernel->get_false() );

        m_kernel = kernel;

        s_androidNativePythonModule = this;

        m_eventation->setEventHandler( PythonEventHandlerInterfacePtr::from( this ) );

        AndroidPlatformExtensionInterface * extension = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        extension->addAndroidEventation( m_eventation );

        m_jenv = extension->getJENV();
        jobject jActivity = extension->getJObjectActivity();

        m_jclass_Object = m_jenv->FindClass( "java/lang/Object" );
        m_jclass_Boolean = m_jenv->FindClass( "java/lang/Boolean" );
        m_jclass_Character = m_jenv->FindClass( "java/lang/Character" );
        m_jclass_Integer = m_jenv->FindClass( "java/lang/Integer" );
        m_jclass_Long = m_jenv->FindClass( "java/lang/Long" );
        m_jclass_Float = m_jenv->FindClass( "java/lang/Float" );
        m_jclass_Double = m_jenv->FindClass( "java/lang/Double" );
        m_jclass_String = m_jenv->FindClass( "java/lang/String" );
        m_jclass_ArrayList = m_jenv->FindClass( "java/util/ArrayList" );

        AndroidPlatformExtensionInterface * unknownAndroidPlatform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

        jclass jclassActivity = unknownAndroidPlatform->getJClassActivity();

        jmethodID jmethodID_initializePlugins = m_jenv->GetMethodID( jclassActivity, "pythonInitializePlugins", "()V" );

        MENGINE_ASSERTION_FATAL( jmethodID_initializePlugins != nullptr, "invalid get android method 'pythonInitializePlugins'" );

        m_jenv->CallVoidMethod( jActivity, jmethodID_initializePlugins );

        m_eventation->invoke();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::_finalizeModule()
    {
        m_globals.reset();

        s_androidNativePythonModule = nullptr;

        m_callbacks.clear();
        m_plugins.clear();

        AndroidPlatformExtensionInterface * extension = PLATFORM_SERVICE()
                ->getDynamicUnknown();

        extension->removeAndroidEventation( m_eventation );

        m_eventation->finalize();
        m_eventation = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::addCommand( const LambdaPythonEventHandler & _command )
    {
        m_eventation->addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::_update( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        m_eventation->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonModule::getPythonAttribute( jobject obj )
    {
        PyObject * py_value = nullptr;

        if( obj == nullptr )
        {
            py_value = m_kernel->ret_none();
        }
        else if( m_jenv->IsInstanceOf( obj, m_jclass_Boolean ) == JNI_TRUE )
        {
            static jmethodID methodValue = m_jenv->GetMethodID( m_jclass_Boolean, "booleanValue", "()Z" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr );

            jboolean value = m_jenv->CallBooleanMethod( obj, methodValue );

            py_value = m_kernel->ret_bool( value );
        }
        else if( m_jenv->IsInstanceOf( obj, m_jclass_Character ) == JNI_TRUE )
        {
            static jmethodID methodValue = m_jenv->GetMethodID( m_jclass_Character, "charValue", "()C" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr );

            jchar value = m_jenv->CallCharMethod( obj, methodValue );

            Char value_str[2] = {(Char)value, '\0'};
            py_value = m_kernel->string_from_char_size( value_str, 1 );
        }
        else if( m_jenv->IsInstanceOf( obj, m_jclass_Integer ) == JNI_TRUE )
        {
            static jmethodID methodValue = m_jenv->GetMethodID( m_jclass_Integer, "intValue", "()I" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr );

            jint value = m_jenv->CallIntMethod( obj, methodValue );

            py_value = m_kernel->ptr_int32( value );
        }
        else if( m_jenv->IsInstanceOf( obj, m_jclass_Long ) == JNI_TRUE )
        {
            static jmethodID methodValue = m_jenv->GetMethodID( m_jclass_Long, "longValue", "()J" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr );

            jlong value = m_jenv->CallLongMethod( obj, methodValue );

            py_value = m_kernel->ptr_int64( value );
        }
        else if( m_jenv->IsInstanceOf( obj, m_jclass_Float ) == JNI_TRUE )
        {
            static jmethodID methodValue = m_jenv->GetMethodID( m_jclass_Float, "floatValue", "()F" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr );

            jfloat value = m_jenv->CallFloatMethod( obj, methodValue );

            py_value = m_kernel->ptr_float( value );
        }
        else if( m_jenv->IsInstanceOf( obj, m_jclass_Double ) == JNI_TRUE )
        {
            static jmethodID methodValue = m_jenv->GetMethodID( m_jclass_Double, "doubleValue", "()D" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr );

            jfloat value = m_jenv->CallDoubleMethod( obj, methodValue );

            py_value = m_kernel->ptr_float( value );
        }
        else if( m_jenv->IsInstanceOf( obj, m_jclass_String ) == JNI_TRUE )
        {
            const Char * obj_str = m_jenv->GetStringUTFChars( (jstring)obj, NULL);

            py_value = m_kernel->string_from_char( obj_str );

            m_jenv->ReleaseStringUTFChars( (jstring)obj, obj_str );
        }
        else if( m_jenv->IsInstanceOf( obj, m_jclass_ArrayList ) == JNI_TRUE )
        {
            static jfieldID fieldElementData = m_jenv->GetFieldID( m_jclass_ArrayList, "elementData", "[Ljava/lang/Object;" );

            MENGINE_ASSERTION_FATAL( fieldElementData != nullptr );

            jobjectArray list_elementData = (jobjectArray)m_jenv->GetObjectField( obj, fieldElementData );

            jsize list_size = m_jenv->GetArrayLength( list_elementData );

            PyObject * py_list = m_kernel->tuple_new( list_size );

            for( jsize index = 0; index != list_size; ++index )
            {
                jobject list_obj = m_jenv->GetObjectArrayElement( list_elementData, index );

                PyObject * py_obj = this->getPythonAttribute( list_obj );

                MENGINE_ASSERTION_FATAL( py_obj != nullptr );

                m_kernel->tuple_setitem( py_list, index, py_obj );
            }

            py_value = py_list;
        }
        else
        {
            jclass cls_obj = m_jenv->GetObjectClass( obj );

            jmethodID mid_getClass = m_jenv->GetMethodID( cls_obj, "getClass", "()Ljava/lang/Class;" );
            jobject obj_class = m_jenv->CallObjectMethod( obj, mid_getClass );

            jclass cls_class = m_jenv->GetObjectClass( obj_class );

            jmethodID mid_getName = m_jenv->GetMethodID( cls_class, "getName", "()Ljava/lang/String;" );

            jstring obj_class_name = (jstring)m_jenv->CallObjectMethod( obj_class, mid_getName );

            const Char * obj_class_name_str = m_jenv->GetStringUTFChars( obj_class_name, NULL);

            LOGGER_ERROR( "unsuported java argument type '%s'"
                , obj_class_name_str
            );

            m_jenv->ReleaseStringUTFChars( obj_class_name, obj_class_name_str );
        }

        return py_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::pythonMethod( const String & _plugin, const String & _method, int32_t _id, jobjectArray _args )
    {
        m_eventation->invoke();

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
            m_jenv->DeleteGlobalRef( _args );

            return;
        }

        jsize args_size = m_jenv->GetArrayLength( _args );

        PyObject * py_args = m_kernel->tuple_new( args_size );

        for( jsize index = 0; index != args_size; ++index )
        {
            jobject obj = m_jenv->GetObjectArrayElement( _args, index );

            PyObject * py_arg = this->getPythonAttribute( obj );

            MENGINE_ASSERTION_FATAL( py_arg != nullptr, "android plugin '%s' method '%s' id '%d' invalid arg"
                , _plugin.c_str()
                , _method.c_str()
                , _id
            );

            m_kernel->tuple_setitem( py_args, index, py_arg );
        }

        m_jenv->DeleteGlobalRef( _args );

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

        this->androidResponse( _id, py_result );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::addPlugin( const String & _name, const jobject & _plugin )
    {
        MENGINE_ASSERTION_FATAL( m_plugins.find( Helper::stringizeString( _name ) ) == m_plugins.end(), "invalid add plugin '%s' [double]"
            , _name.c_str()
        );

        SCRIPT_SERVICE()
            ->setAvailablePlugin( _name.c_str(), true );

        m_plugins.emplace( Helper::stringizeString( _name ), _plugin );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::setAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb )
    {
        MENGINE_ASSERTION_FATAL( m_callbacks.find( Helper::makePair( _plugin, _method ) ) == m_callbacks.end(), "invalid add plugin '%s' callback '%s' [double]"
            , _plugin.c_str()
            , _method.c_str()
        );

        m_callbacks.emplace( Helper::makePair( _plugin, _method ), _cb );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonModule::androidResponse( int32_t _id, const pybind::object & _result ) const
    {
        m_eventation->invoke();

        jobject jresult;

        if( _result.is_none() == true )
        {
            static jmethodID constructor = m_jenv->GetMethodID( m_jclass_Object, "<init>", "()V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object()V'" );

            jresult = m_jenv->NewObject( m_jclass_Object, constructor );
        }
        else if( _result.is_bool() == true )
        {
            static jmethodID constructor = m_jenv->GetMethodID( m_jclass_Object, "<init>", "(Z)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object(Z)V'" );

            jresult = m_jenv->NewObject( m_jclass_Object, constructor, (bool)_result.extract() );
        }
        else if( _result.is_integer() == true )
        {
            static jmethodID constructor = m_jenv->GetMethodID( m_jclass_Object, "<init>", "(I)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object(I)V'" );

            jresult = m_jenv->NewObject( m_jclass_Object, constructor, (int32_t)_result.extract() );
        }
        else if( _result.is_long() == true )
        {
            static jmethodID constructor = m_jenv->GetMethodID( m_jclass_Object, "<init>", "(L)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object(L)V'" );

            jresult = m_jenv->NewObject( m_jclass_Object, constructor, (int64_t)_result.extract() );
        }
        else if( _result.is_float() == true )
        {
            static jmethodID constructor = m_jenv->GetMethodID( m_jclass_Object, "<init>", "(F)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object(F)V'" );

            jresult = m_jenv->NewObject( m_jclass_Object, constructor, (float)_result.extract() );
        }
        else if( _result.is_string() == true )
        {
            const Char * value_str = (const Char *)_result.extract();

            jstring jvalue = m_jenv->NewStringUTF( value_str );

            static jmethodID constructor = m_jenv->GetMethodID( m_jclass_Object, "<init>", "(Ljava/lang/String;)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object(Ljava/lang/String;)V'" );

            jresult = m_jenv->NewObject( m_jclass_Object, constructor, jvalue );

            m_jenv->DeleteLocalRef( jvalue );
        }
        else
        {
            LOGGER_ERROR( "android response call '%d' unsupported result type [%s]"
                , _id
                , _result.repr_type().c_str()
            );

            return false;
        }

        AndroidPlatformExtensionInterface * extension = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        jclass jclass_activity = extension->getJClassActivity();

        static jmethodID jmethodID_responseCall = m_jenv->GetMethodID( jclass_activity, "responseCall", "(ILjava/lang/Object;)V" );

        MENGINE_ASSERTION_FATAL( jmethodID_responseCall != 0, "android activity not found method 'responseCall'" );

        jobject jobject_activity = extension->getJObjectActivity();

        m_jenv->CallVoidMethod( jobject_activity, jmethodID_responseCall, _id, jresult );

        m_eventation->invoke();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [void]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( _plugin, _method, _args, "V", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return;
        }

        m_jenv->CallVoidMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            m_jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonModule::androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [boolean]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( _plugin, _method, _args, "Z", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return false;
        }

        jboolean jresult = m_jenv->CallBooleanMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            m_jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (bool)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidNativePythonModule::androidInteger32Method( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [int]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( _plugin, _method, _args, "I", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return false;
        }

        jint jresult = m_jenv->CallBooleanMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            m_jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (int32_t)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidNativePythonModule::androidInteger64Method( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [long]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( _plugin, _method, _args, "J", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return false;
        }

        jlong jresult = m_jenv->CallBooleanMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            m_jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (int64_t)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidNativePythonModule::androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [float]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( _plugin, _method, _args, "F", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0.0;
        }

        jfloat jresult = m_jenv->CallFloatMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            m_jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (float)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    String AndroidNativePythonModule::androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [string]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( _plugin, _method, _args, "Ljava/lang/String;", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return String();
        }

        jstring jresult = (jstring)m_jenv->CallObjectMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            m_jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        const Mengine::Char * result_str = m_jenv->GetStringUTFChars( jresult, nullptr );

        Mengine::String result = result_str;

        m_jenv->ReleaseStringUTFChars( jresult, result_str );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonModule::getAndroidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jfree, uint32_t * const _freeCount, jobject * const _jplugin, jmethodID * const _jmethodId ) const
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

        jclass plugin_class = m_jenv->GetObjectClass( jplugin );

        if( jplugin == nullptr )
        {
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

                jstring jvalue = m_jenv->NewStringUTF( value_str );

                _jargs[index_args++].l = jvalue;
                _jfree[index_free++] = jvalue;

                MENGINE_STRCAT( signature, "Ljava/lang/String;" );
            }
            else if( arg.is_list() == true )
            {
                pybind::list l = arg.extract();

                pybind::list::size_type s = l.size();

                static jmethodID jmethodID_List_constructor = m_jenv->GetMethodID( m_jclass_ArrayList, "<init>", "(I)V" );

                MENGINE_ASSERTION_FATAL( jmethodID_List_constructor != nullptr );

                jobject jlist = m_jenv->NewObject( m_jclass_ArrayList, jmethodID_List_constructor, s);

                uint32_t index = 0;
                for( const pybind::object & o : l )
                {
                    const Char * o_str = o.extract();

                    jstring jelement = m_jenv->NewStringUTF( o_str );

                    static jmethodID jmethodID_List_add = m_jenv->GetMethodID( m_jclass_ArrayList, "add", "(Ljava/lang/Object;)Z" );

                    MENGINE_ASSERTION_FATAL( jmethodID_List_add != nullptr );

                    jboolean result = m_jenv->CallBooleanMethod( jlist, jmethodID_List_add, jelement );

                    m_jenv->DeleteLocalRef( jelement );

                    ++index;
                }

                _jargs[index_args++].l = jlist;
                _jfree[index_free++] = jlist;

                MENGINE_STRCAT( signature, "Ljava/util/List;" );
            }
            else
            {
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

        jmethodID jmethodId = m_jenv->GetMethodID( plugin_class, _method.c_str(), signature );

        if( jmethodId == nullptr )
        {
            LOGGER_ERROR( "android plugin '%s' not found method '%s' with signature '%s'"
                , _plugin.c_str()
                , _method.c_str()
                , signature
            );

            return false;
        }

        *_freeCount = index_free;
        *_jplugin = jplugin;
        *_jmethodId = jmethodId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
