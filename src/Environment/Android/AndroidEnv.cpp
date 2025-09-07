#include "AndroidEnv.h"

#include "AndroidDeclaration.h"

#include "Kernel/BuildMode.h"
#include "Kernel/Assertion.h"

#include "Config/StdString.h"
#include "Config/StdLib.h"
#include "Config/StdArg.h"

#include <signal.h>
#include <pthread.h>

static JavaVM * g_androidEnvJavaVM = nullptr;

static jobject g_jobject_MengineClassLoader = nullptr;

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1setMengineAndroidClassLoaderJNI )( JNIEnv * env, jclass cls, jobject cl )
    {
        g_jobject_MengineClassLoader = (jobject)env->NewGlobalRef( cl );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1removeMengineAndroidClassLoaderJNI )( JNIEnv * env, jclass cls )
    {
        env->DeleteGlobalRef( g_jobject_MengineClassLoader );
        g_jobject_MengineClassLoader = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1isMasterRelease )( JNIEnv *, jclass cls )
    {
        bool mode = Mengine::Helper::isMasterRelease();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1getEngineGITSHA1 )( JNIEnv * env, jclass cls )
    {
        const Mengine::Char * ENGINE_GIT_SHA1 = Mengine::Helper::getEngineGitSHA1();

        jstring result = env->NewStringUTF( ENGINE_GIT_SHA1 );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1getEngineVersion )( JNIEnv * env, jclass cls )
    {
        const Mengine::Char * ENGINE_VERSION = Mengine::Helper::getEngineVersion();

        jstring result = env->NewStringUTF( ENGINE_VERSION );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1getBuildDate )( JNIEnv * env, jclass cls )
    {
        const Mengine::Char * BUILD_DATE = Mengine::Helper::getBuildDate();

        jstring result = env->NewStringUTF( BUILD_DATE );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1getBuildUsername )( JNIEnv * env, jclass cls )
    {
        const Mengine::Char * BUILD_USERNAME = Mengine::Helper::getBuildUsername();

        jstring result = env->NewStringUTF( BUILD_USERNAME );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM * vm, void * reserved )
    {
        MENGINE_UNUSED( reserved );

        g_androidEnvJavaVM = vm;

        JNIEnv * env;
        int status = g_androidEnvJavaVM->GetEnv( (void **)&env, JNI_VERSION_1_6 );

        if( status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_OnLoad failed to get JNI Env [%d]", status );

            return JNI_ERR;
        }

        __android_log_print( ANDROID_LOG_INFO, "Mengine", "JNI_OnLoad" );

        return JNI_VERSION_1_6;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL JNI_OnUnload( JavaVM * vm, void * reserved )
    {
        JNIEnv * env;
        int status = vm->GetEnv( (void**)&env, JNI_VERSION_1_6 );

        if( status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_OnUnload failed to get JNI Env [%d]", status );

            return;
        }

        __android_log_print( ANDROID_LOG_INFO, "Mengine", "JNI_OnUnload" );
    }
    //////////////////////////////////////////////////////////////////////////
}

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static pthread_key_t g_androidEnvThreadKey = 0;
    //////////////////////////////////////////////////////////////////////////
    static void Mengine_JNI_ThreadDestroyed( void * _value )
    {
        JNIEnv * env = (JNIEnv *)_value;

        if( env == nullptr )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_ThreadDestroyed called with null JNIEnv" );

            return;
        }

        int status = ::pthread_setspecific( g_androidEnvThreadKey, nullptr );

        if( status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_ThreadDestroyed failed to clear pthread key [%d]", status );
        }

        if( g_androidEnvJavaVM == nullptr )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_ThreadDestroyed failed to get JavaVM" );

            return;
        }

        g_androidEnvJavaVM->DetachCurrentThread();
    }
    //////////////////////////////////////////////////////////////////////////
    static int Mengine_JNI_SetEnv( JNIEnv * _env )
    {
        MENGINE_ASSERTION_FATAL( g_androidEnvThreadKey != 0, "android ENV thread key not initialized" );
        MENGINE_ASSERTION_FATAL( pthread_getspecific( g_androidEnvThreadKey ) == nullptr, "ENV thread key already set" );

        int status = ::pthread_setspecific( g_androidEnvThreadKey, (void *)_env );

        if( status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetEnv failed to set pthread key [%d]", status );

            return JNI_FALSE;
        }

        __android_log_print( ANDROID_LOG_INFO, "Mengine", "JNI_SetEnv set JNIEnv for current thread" );

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    int Mengine_JNI_Initialize( JNIEnv * _env )
    {
        MENGINE_ASSERTION_FATAL( g_androidEnvThreadKey == 0, "android ENV thread key already initialize" );

        int status = ::pthread_key_create( &g_androidEnvThreadKey, &Mengine_JNI_ThreadDestroyed );

        if( status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetEnv failed to create pthread key [%d]", status );

            return JNI_FALSE;
        }

        __android_log_print( ANDROID_LOG_INFO, "Mengine", "JNI_SetEnv created pthread key for JNIEnv" );

        if( Mengine_JNI_SetEnv( _env ) == JNI_FALSE )
        {
            return JNI_FALSE;
        }

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEnv * Mengine_JNI_GetEnv()
    {
        MENGINE_ASSERTION_FATAL( g_androidEnvThreadKey != 0, "android ENV thread key not initialized" );

        JNIEnv * jenv = (JNIEnv *)::pthread_getspecific(g_androidEnvThreadKey );

        if( jenv != nullptr )
        {
            return jenv;
        }

        if( g_androidEnvJavaVM == nullptr )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_GetEnv failed to get JavaVM" );

            return nullptr;
        }

        JNIEnv * new_jenv;
        jint status = g_androidEnvJavaVM->AttachCurrentThread( &new_jenv, nullptr );

        if( status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_GetEnv failed to attach current thread [%d]", status );

            return nullptr;
        }

        if( Mengine_JNI_SetEnv( new_jenv ) == JNI_FALSE )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_GetEnv failed to set environment" );

            g_androidEnvJavaVM->DetachCurrentThread();

        	return nullptr;
        }

        return new_jenv;
    }
    //////////////////////////////////////////////////////////////////////////
    int Mengine_JNI_SetupThread()
    {
        if( g_androidEnvJavaVM == nullptr )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetupThread failed to get JavaVM" );

            return JNI_FALSE;
        }

        JNIEnv * env;
        jint status = g_androidEnvJavaVM->AttachCurrentThread( &env, nullptr );

        if( status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetupThread failed to attach current thread [%d]", status );

            return JNI_FALSE;
        }

        if( Mengine_JNI_SetEnv( env ) == JNI_FALSE )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetupThread failed to set environment" );

            g_androidEnvJavaVM->DetachCurrentThread();

            return JNI_FALSE;
        }

        __android_log_print( ANDROID_LOG_INFO, "Mengine", "JNI_SetupThread successfully set up thread environment" );

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_IsInstanceOf( JNIEnv * _jenv, jobject _jobject, jclass _jclass )
    {
        jboolean result = _jenv->IsInstanceOf( _jobject, _jclass );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassBoolean( JNIEnv * _jenv )
    {
        jclass jclass_Boolean = _jenv->FindClass( "java/lang/Boolean" );

        return jclass_Boolean;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassCharacter( JNIEnv * _jenv )
    {
        jclass jclass_Character = _jenv->FindClass( "java/lang/Character" );

        return jclass_Character;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassInteger( JNIEnv * _jenv )
    {
        jclass jclass_Integer = _jenv->FindClass( "java/lang/Integer" );

        return jclass_Integer;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassLong( JNIEnv * _jenv )
    {
        jclass jclass_Long = _jenv->FindClass( "java/lang/Long" );

        return jclass_Long;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassFloat( JNIEnv * _jenv )
    {
        jclass jclass_Float = _jenv->FindClass( "java/lang/Float" );

        return jclass_Float;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassDouble( JNIEnv * _jenv )
    {
        jclass jclass_Double = _jenv->FindClass( "java/lang/Double" );

        return jclass_Double;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassString( JNIEnv * _jenv )
    {
        jclass jclass_String = _jenv->FindClass( "java/lang/String" );

        return jclass_String;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassException( JNIEnv * _jenv )
    {
        jclass jclass_Exception = _jenv->FindClass( "java/lang/Exception" );

        return jclass_Exception;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassList( JNIEnv * _jenv )
    {
        jclass jclass_List = _jenv->FindClass( "java/util/List" );

        return jclass_List;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassMap( JNIEnv * _jenv )
    {
        jclass jclass_Map = _jenv->FindClass( "java/util/Map" );

        return jclass_Map;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassSet( JNIEnv * _jenv )
    {
        jclass jclass_Set = _jenv->FindClass( "java/util/Set" );

        return jclass_Set;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassArrayList( JNIEnv * _jenv )
    {
        jclass jclass_ArrayList = _jenv->FindClass( "java/util/ArrayList" );

        return jclass_ArrayList;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassHashMap( JNIEnv * _jenv )
    {
        jclass jclass_HashMap = _jenv->FindClass( "java/util/HashMap" );

        return jclass_HashMap;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassMapEntry( JNIEnv * _jenv )
    {
        jclass jclass_MapEntry = _jenv->FindClass( "java/util/Map$Entry" );

        return jclass_MapEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassIterator( JNIEnv * _jenv )
    {
        jclass jclass_Iterator = _jenv->FindClass( "java/util/Iterator" );

        return jclass_Iterator;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassRect( JNIEnv * _jenv )
    {
        jclass jclass_Rect = _jenv->FindClass( "android/graphics/Rect" );

        return jclass_Rect;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassJSONObject( JNIEnv * _jenv )
    {
        jclass jclass_JSONObject = _jenv->FindClass( "org/json/JSONObject" );

        return jclass_JSONObject;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassJSONArray( JNIEnv * _jenv )
    {
        jclass jclass_JSONArray = _jenv->FindClass( "org/json/JSONArray" );

        return jclass_JSONArray;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassClass( JNIEnv * _jenv )
    {
        jclass jclass_Class = _jenv->FindClass( "java/lang/Class" );

        return jclass_Class;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassClassLoader( JNIEnv * _jenv )
    {
        jclass jclass_ClassLoader = _jenv->FindClass( "java/lang/ClassLoader" );

        return jclass_ClassLoader;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetObjectClass( JNIEnv * _jenv, jobject _jobject )
    {
        jclass jclass_Object = _jenv->GetObjectClass( _jobject );

        return jclass_Object;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_DeleteLocalRef( JNIEnv * _jenv, jobject _jobject )
    {
        _jenv->DeleteLocalRef( _jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_NewGlobalRef( JNIEnv * _jenv, jobject _jobject )
    {
        jobject jobjectGlobal = _jenv->NewGlobalRef( _jobject );

        return jobjectGlobal;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_DeleteGlobalRef( JNIEnv * _jenv, jobject _jobject )
    {
        _jenv->DeleteGlobalRef( _jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    int Mengine_JNI_ExceptionCheck( JNIEnv * _jenv )
    {
        int result = _jenv->ExceptionCheck();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    jthrowable Mengine_JNI_ExceptionOccurred( JNIEnv * _jenv )
    {
        jthrowable jThrowable = _jenv->ExceptionOccurred();

        return jThrowable;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_ExceptionClear( JNIEnv * _jenv )
    {
        _jenv->ExceptionClear();
    }
    //////////////////////////////////////////////////////////////////////////
    int Mengine_JNI_PushLocalFrame( JNIEnv * _jenv, jint _capacity )
    {
        int status = _jenv->PushLocalFrame( _capacity );

        return status;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_PopLocalFrame( JNIEnv * _jenv, jobject _result )
    {
        jobject jobjectResult = _jenv->PopLocalFrame( _result );

        return jobjectResult;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_FindClass( JNIEnv * _jenv, const Char * _name )
    {
        jclass jclass_FindClass = _jenv->FindClass( _name );

        return jclass_FindClass;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetMethodID( JNIEnv * _jenv, jclass _jclass, const Char * _name, const Char * _signature )
    {
        jmethodID jmethodID = _jenv->GetMethodID( _jclass, _name, _signature );

        return jmethodID;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetStaticMethodID( JNIEnv * _jenv, jclass _jclass, const Char * _name, const Char * _signature )
    {
        jmethodID jmethodID = _jenv->GetStaticMethodID( _jclass, _name, _signature );

        return jmethodID;
    }
    //////////////////////////////////////////////////////////////////////////
    jfieldID Mengine_JNI_GetFieldID( JNIEnv * _jenv, jclass _jclass, const Char * _name, const Char * _signature )
    {
        jfieldID jfieldID = _jenv->GetFieldID( _jclass, _name, _signature );

        return jfieldID;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_GetIntField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        jint jintField = _jenv->GetIntField( _jobject, _fieldID );

        return jintField;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_GetBooleanField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        jboolean jbooleanField = _jenv->GetBooleanField( _jobject, _fieldID );

        return jbooleanField;
    }
    //////////////////////////////////////////////////////////////////////////
    jchar Mengine_JNI_GetCharField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        jchar jcharField = _jenv->GetCharField( _jobject, _fieldID );

        return jcharField;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_GetLongField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        jlong jlongField = _jenv->GetLongField( _jobject, _fieldID );

        return jlongField;
    }
    //////////////////////////////////////////////////////////////////////////
    jfloat Mengine_JNI_GetFloatField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        jfloat jfloatField = _jenv->GetFloatField( _jobject, _fieldID );

        return jfloatField;
    }
    //////////////////////////////////////////////////////////////////////////
    jdouble Mengine_JNI_GetDoubleField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        jdouble jdoubleField = _jenv->GetDoubleField( _jobject, _fieldID );

        return jdoubleField;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        jobject jobjectField = _jenv->GetObjectField( _jobject, _fieldID );

        return jobjectField;
    }
    //////////////////////////////////////////////////////////////////////////
    jfieldID Mengine_JNI_GetStaticFieldID( JNIEnv * _jenv, jclass _jclass, const Char * _name, const Char * _signature )
    {
        jfieldID jfieldID = _jenv->GetStaticFieldID( _jclass, _name, _signature );

        return jfieldID;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetStaticObjectField( JNIEnv * _jenv, jclass _jclass, jfieldID _fieldID )
    {
        jobject jobjectField = _jenv->GetStaticObjectField( _jclass, _fieldID );

        return jobjectField;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallVoidMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        Mengine_JNI_CallVoidMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallVoidMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        _jenv->CallVoidMethodV( _jobject, _methodID, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallVoidMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        _jenv->CallVoidMethodA( _jobject, _methodID, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallBooleanMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jboolean jboolean_result = Mengine_JNI_CallBooleanMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallBooleanMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        jboolean jboolean_result = _jenv->CallBooleanMethodV( _jobject, _methodID, _args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallBooleanMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        jboolean jboolean_result = _jenv->CallBooleanMethodA( _jobject, _methodID, _args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jchar Mengine_JNI_CallCharMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jchar jchar_result = Mengine_JNI_CallCharMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jchar_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jchar Mengine_JNI_CallCharMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        jchar jchar_result = _jenv->CallCharMethodV( _jobject, _methodID, _args );

        return jchar_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jchar Mengine_JNI_CallCharMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        jchar jchar_result = _jenv->CallCharMethodA( _jobject, _methodID, _args );

        return jchar_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallIntMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jint jint_result = Mengine_JNI_CallIntMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallIntMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        jint jint_result = _jenv->CallIntMethodV( _jobject, _methodID, _args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallIntMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        jint jint_result = _jenv->CallIntMethodA( _jobject, _methodID, _args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallLongMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jlong jlong_result = Mengine_JNI_CallLongMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallLongMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        jlong jlong_result = _jenv->CallLongMethodV( _jobject, _methodID, _args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallLongMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        jlong jlong_result = _jenv->CallLongMethodA( _jobject, _methodID, _args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jfloat Mengine_JNI_CallFloatMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jfloat jfloat_result = Mengine_JNI_CallFloatMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jfloat_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jfloat Mengine_JNI_CallFloatMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        jfloat jfloat_result = _jenv->CallFloatMethodV( _jobject, _methodID, _args );

        return jfloat_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jfloat Mengine_JNI_CallFloatMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        jfloat jfloat_result = _jenv->CallFloatMethodA( _jobject, _methodID, _args );

        return jfloat_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jdouble Mengine_JNI_CallDoubleMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jdouble jdouble_result = Mengine_JNI_CallDoubleMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jdouble_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jdouble Mengine_JNI_CallDoubleMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        jdouble jdouble_result = _jenv->CallDoubleMethodV( _jobject, _methodID, _args );

        return jdouble_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jdouble Mengine_JNI_CallDoubleMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        jdouble jdouble_result = _jenv->CallDoubleMethodA( _jobject, _methodID, _args );

        return jdouble_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallObjectMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jobject jobject_result = Mengine_JNI_CallObjectMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallObjectMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        jobject jobject_result = _jenv->CallObjectMethodV( _jobject, _methodID, _args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallObjectMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        jobject jobject_result = _jenv->CallObjectMethodA( _jobject, _methodID, _args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallStaticVoidMethod( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        Mengine_JNI_CallStaticVoidMethodV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallStaticVoidMethodV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        _jenv->CallStaticVoidMethodV( _jclass, _methodID, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallStaticVoidMethodA( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args )
    {
        _jenv->CallStaticVoidMethodA( _jclass, _methodID, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallStaticIntMethod( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jint jint_result = Mengine_JNI_CallStaticIntMethodV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallStaticIntMethodV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        jint jint_result = _jenv->CallStaticIntMethodV( _jclass, _methodID, _args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallStaticIntMethodA( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args )
    {
        jint jint_result = _jenv->CallStaticIntMethodA( _jclass, _methodID, _args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallStaticLongMethod( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jlong jlong_result = Mengine_JNI_CallStaticLongMethodV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallStaticLongMethodV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        jlong jlong_result = _jenv->CallStaticLongMethodV( _jclass, _methodID, _args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallStaticLongMethodA( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args )
    {
        jlong jlong_result = _jenv->CallStaticLongMethodA( _jclass, _methodID, _args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallStaticBooleanMethod( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jboolean jboolean_result = Mengine_JNI_CallStaticBooleanMethodV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallStaticObjectMethod( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jobject jobject_result = Mengine_JNI_CallStaticObjectMethodV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallStaticBooleanMethodV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        jboolean jboolean_result = _jenv->CallStaticBooleanMethodV( _jclass, _methodID, _args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallStaticBooleanMethodA( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args )
    {
        jboolean jboolean_result = _jenv->CallStaticBooleanMethodA( _jclass, _methodID, _args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallStaticObjectMethodV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        jobject jobject_result = _jenv->CallStaticObjectMethodV( _jclass, _methodID, _args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallStaticObjectMethodA( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args )
    {
        jobject jobject_result = _jenv->CallStaticObjectMethodA( _jclass, _methodID, _args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_NewObject( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jobject jobject_result = Mengine_JNI_NewObjectV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_NewObjectV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        jobject jobject_result = _jenv->NewObjectV( _jclass, _methodID, _args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jstring Mengine_JNI_NewStringUTF( JNIEnv * _jenv, const Char * _str )
    {
        jstring jstr = _jenv->NewStringUTF( _str );

        return jstr;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Mengine_JNI_GetStringUTFChars( JNIEnv * _jenv, jstring _jstring, jboolean * const _isCopy )
    {
        const Char * str = _jenv->GetStringUTFChars( _jstring, _isCopy );

        return str;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_ReleaseStringUTFChars( JNIEnv * _jenv, jstring _jstring, const Char * _cstr )
    {
        _jenv->ReleaseStringUTFChars( _jstring, _cstr );
    }
    //////////////////////////////////////////////////////////////////////////
    jsize Mengine_JNI_GetStringLength( JNIEnv * _jenv, jstring _jstring )
    {
        jsize length = _jenv->GetStringLength( _jstring );

        return length;
    }
    //////////////////////////////////////////////////////////////////////////
    jsize Mengine_JNI_GetArrayLength( JNIEnv * _jenv, jobjectArray _jarray )
    {
        jsize length = _jenv->GetArrayLength( _jarray );

        return length;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectArrayElement( JNIEnv * _jenv, jobjectArray _jarray, jsize _index )
    {
        jobject jobject_element = _jenv->GetObjectArrayElement( _jarray, _index );

        return jobject_element;
    }
    //////////////////////////////////////////////////////////////////////////
    jbyteArray Mengine_JNI_NewByteArray( JNIEnv * _jenv, jsize _length )
    {
        jbyteArray jbyte_array = _jenv->NewByteArray( _length );

        return jbyte_array;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_SetByteArrayRegion( JNIEnv * _jenv, jbyteArray _jarray, jsize _start, jsize _len, const jbyte * _buf )
    {
        _jenv->SetByteArrayRegion( _jarray, _start, _len, _buf );
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_ReleaseByteArrayElements( JNIEnv * _jenv, jbyteArray _jarray, jbyte * _elements, jint _mode )
    {
        _jenv->ReleaseByteArrayElements( _jarray, _elements, _mode );
    }
    //////////////////////////////////////////////////////////////////////////
    jbyte * Mengine_JNI_GetByteArrayElements( JNIEnv * _jenv, jbyteArray _jarray, jboolean * const _isCopy )
    {
        jbyte * byte_elements = _jenv->GetByteArrayElements( _jarray, _isCopy );

        return byte_elements;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_NewDirectByteBuffer( JNIEnv * _jenv, void * _address, jlong _capacity )
    {
        jobject jbyte_buffer = _jenv->NewDirectByteBuffer( _address, _capacity );

        return jbyte_buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void * Mengine_JNI_GetDirectBufferAddress( JNIEnv * _jenv, jobject _jbuffer )
    {
        void * address = _jenv->GetDirectBufferAddress( _jbuffer );

        return address;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassMengineCallback( JNIEnv * _jenv )
    {
        jclass jclass_MengineCallback = Mengine_JNI_LoadClass( _jenv, "org/Mengine/Base/MengineCallback" );

        return jclass_MengineCallback;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassApplication( JNIEnv * _jenv )
    {
        jclass jclass_MengineApplication = Mengine_JNI_LoadClass( _jenv, "org/Mengine/Base/MengineApplication" );

        return jclass_MengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectApplication( JNIEnv * _jenv )
    {
        jclass jclass_MengineApplication = Mengine_JNI_GetClassApplication( _jenv );

        jfieldID jfield_INSTANCE = Mengine_JNI_GetStaticFieldID( _jenv, jclass_MengineApplication, "INSTANCE", "Lorg/Mengine/Base/MengineApplication;" );

        if( jfield_INSTANCE == nullptr )
        {
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_MengineApplication );

            return nullptr;
        }

        jobject jobject_MengineApplication = Mengine_JNI_GetStaticObjectField( _jenv, jclass_MengineApplication, jfield_INSTANCE );

        Mengine_JNI_DeleteLocalRef( _jenv, jclass_MengineApplication );

        return jobject_MengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetMethodApplication( JNIEnv * _jenv, const char * _methodName, const char * _signature )
    {
        jclass jclass_MengineApplication = Mengine_JNI_GetClassApplication( _jenv );

        jmethodID jmethod_MengineApplication = Mengine_JNI_GetMethodID( _jenv, jclass_MengineApplication, _methodName, _signature );

        Mengine_JNI_DeleteLocalRef( _jenv, jclass_MengineApplication );

        return jmethod_MengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassActivity( JNIEnv * _jenv )
    {
        jclass jclass_MengineActivity = Mengine_JNI_LoadClass( _jenv, "org/Mengine/Base/MengineActivity" );

        return jclass_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectActivity( JNIEnv * _jenv )
    {
        jclass jclass_MengineActivity = Mengine_JNI_GetClassActivity( _jenv );

        jfieldID jfield_INSTANCE = Mengine_JNI_GetStaticFieldID( _jenv, jclass_MengineActivity, "INSTANCE", "Lorg/Mengine/Base/MengineActivity;" );

        if( jfield_INSTANCE == nullptr )
        {
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_MengineActivity );

            return nullptr;
        }

        jobject jobject_MengineActivity = Mengine_JNI_GetStaticObjectField( _jenv,  jclass_MengineActivity, jfield_INSTANCE );

        Mengine_JNI_DeleteLocalRef( _jenv, jclass_MengineActivity );

        return jobject_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetMethodActivity( JNIEnv * _jenv, const Char * _methodName, const Char * _signature )
    {
        jclass jclass_MengineActivity = Mengine_JNI_GetClassActivity( _jenv );

        jmethodID jmethodId_MengineActivity = Mengine_JNI_GetMethodID( _jenv, jclass_MengineActivity, _methodName, _signature );

        Mengine_JNI_DeleteLocalRef( _jenv, jclass_MengineActivity );

        return jmethodId_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassFragment( JNIEnv * _jenv, const Char * _fragmentName )
    {
        Char fragmentClassName[256 + 1] = { '\0' };
        StdString::strcpy_safe( fragmentClassName, "org/Mengine/Base/", 256 );
        StdString::strcat_safe( fragmentClassName, _fragmentName, 256 );

        jclass jclass_FragmentClassName = Mengine_JNI_LoadClass( _jenv, fragmentClassName );

        return jclass_FragmentClassName;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectFragment( JNIEnv * _jenv, const Char * _fragmentName )
    {
        jclass jclass_Fragment = Mengine_JNI_GetClassFragment( _jenv, _fragmentName );

        if( jclass_Fragment == nullptr )
        {
            return nullptr;
        }

        Char fragmentSignature[256 + 1] = { '\0' };
        StdString::strcpy_safe( fragmentSignature, "Lorg/Mengine/Base/", 256 );
        StdString::strcat_safe( fragmentSignature, _fragmentName, 256 );
        StdString::strcat_safe( fragmentSignature, ";", 256 );

        jfieldID jfield_INSTANCE = Mengine_JNI_GetStaticFieldID( _jenv, jclass_Fragment, "INSTANCE", fragmentSignature );

        if( jfield_INSTANCE == nullptr )
        {
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Fragment );

            return nullptr;
        }

        jobject jobject_INSTANCE = Mengine_JNI_GetStaticObjectField( _jenv, jclass_Fragment, jfield_INSTANCE );

        Mengine_JNI_DeleteLocalRef( _jenv, jclass_Fragment );

        return jobject_INSTANCE;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetMethodFragment( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature )
    {
        jclass jclass_Fragment = Mengine_JNI_GetClassFragment( _jenv, _fragment );

        if( jclass_Fragment == nullptr )
        {
            return nullptr;
        }

        jmethodID methodId = Mengine_JNI_GetMethodID( _jenv, jclass_Fragment, _method, _signature );

        Mengine_JNI_DeleteLocalRef( _jenv, jclass_Fragment );

        return methodId;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_LoadClass( JNIEnv * _jenv, const Char * _className )
    {
        jclass jclass_ClassLoader = Mengine_JNI_GetClassClassLoader( _jenv );
        jmethodID jmethodID_ClassLoader_loadClass = Mengine_JNI_GetMethodID( _jenv, jclass_ClassLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;" );
        Mengine_JNI_DeleteLocalRef( _jenv, jclass_ClassLoader );

        jstring jstring_className = Mengine_JNI_NewStringUTF( _jenv, _className );
        jclass jclass_FindClass = (jclass)Mengine_JNI_CallObjectMethod( _jenv, g_jobject_MengineClassLoader, jmethodID_ClassLoader_loadClass, jstring_className );
        Mengine_JNI_DeleteLocalRef( _jenv, jstring_className );

        return jclass_FindClass;
    }
    //////////////////////////////////////////////////////////////////////////
    AAssetManager * Mengine_JNI_GetAssetManagerFromJava( JNIEnv * _jenv, jobject _jassetManager )
    {
        AAssetManager * assetManager = AAssetManager_fromJava( _jenv, _jassetManager );

        return assetManager;
    }
    //////////////////////////////////////////////////////////////////////////
    ANativeWindow * Mengine_JNI_ANativeWindow_fromSurface( JNIEnv * _jenv, jobject _surface )
    {
        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( _jenv, _surface );

        return nativeWindow;
    }
    //////////////////////////////////////////////////////////////////////////
}