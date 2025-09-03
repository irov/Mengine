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
        const Mengine::Char * ENGINE_GIT_SHA1 = Mengine::Helper::getEngineGITSHA1();

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
    static int Mengine_JNI_SetEnv( JNIEnv * _env, MengineJNIEnvThread ** _envThread )
    {
        MENGINE_ASSERTION_FATAL( g_androidEnvThreadKey != 0, "android ENV thread key not initialized" );
        MENGINE_ASSERTION_FATAL( pthread_getspecific( g_androidEnvThreadKey ) == nullptr, "ENV thread key already set" );

        MengineJNIEnvThread * envThread = (MengineJNIEnvThread *)StdLib::malloc( sizeof(MengineJNIEnvThread) );

        if( envThread == nullptr )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetEnv failed to allocate memory for MengineJNIEnvThread" );

            return JNI_FALSE;
        }

        envThread->env = _env;
        envThread->jclass_Boolean = _env->FindClass( "java/lang/Boolean" );
        envThread->jclass_Character = _env->FindClass( "java/lang/Character" );
        envThread->jclass_Integer = _env->FindClass( "java/lang/Integer" );
        envThread->jclass_Long = _env->FindClass( "java/lang/Long" );
        envThread->jclass_Float = _env->FindClass( "java/lang/Float" );
        envThread->jclass_Double = _env->FindClass( "java/lang/Double" );
        envThread->jclass_String = _env->FindClass( "java/lang/String" );
        envThread->jclass_Exception = _env->FindClass( "java/lang/Exception" );
        envThread->jclass_List = _env->FindClass( "java/util/List" );
        envThread->jclass_Map = _env->FindClass( "java/util/Map" );
        envThread->jclass_Set = _env->FindClass( "java/util/Set" );
        envThread->jclass_ArrayList = _env->FindClass( "java/util/ArrayList" );
        envThread->jclass_HashMap = _env->FindClass( "java/util/HashMap" );
        envThread->jclass_MapEntry = _env->FindClass( "java/util/Map$Entry" );
        envThread->jclass_Iterator = _env->FindClass( "java/util/Iterator" );
        envThread->jclass_Rect = _env->FindClass( "android/graphics/Rect" );
        envThread->jclass_JSONObject = _env->FindClass( "org/json/JSONObject" );
        envThread->jclass_JSONArray = _env->FindClass( "org/json/JSONArray" );
        envThread->jclass_Class = _env->FindClass( "java/lang/Class" );
        envThread->jclass_ClassLoader = _env->FindClass( "java/lang/ClassLoader" );

        envThread->jclass_MengineCallback = Mengine_JNI_LoadClass( envThread, "org/Mengine/Base/MengineCallback" );
        envThread->jclass_MengineApplication = Mengine_JNI_LoadClass( envThread, "org/Mengine/Base/MengineApplication" );
        envThread->jclass_MengineActivity = Mengine_JNI_LoadClass( envThread, "org/Mengine/Base/MengineActivity" );

        int status = ::pthread_setspecific( g_androidEnvThreadKey, (void *)envThread );

        if( status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetEnv failed to set pthread key [%d]", status );

            return JNI_FALSE;
        }

        __android_log_print( ANDROID_LOG_INFO, "Mengine", "JNI_SetEnv set JNIEnv for current thread" );

        if( _envThread != nullptr )
        {
            *_envThread = envThread;
        }

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

        if( Mengine_JNI_SetEnv( _env, nullptr ) == JNI_FALSE )
        {
            return JNI_FALSE;
        }

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    MengineJNIEnvThread * Mengine_JNI_GetEnvThread()
    {
        MENGINE_ASSERTION_FATAL( g_androidEnvThreadKey != 0, "android ENV thread key not initialized" );

        MengineJNIEnvThread * envThread = (MengineJNIEnvThread *)::pthread_getspecific(g_androidEnvThreadKey );

        if( envThread != nullptr )
        {
            return envThread;
        }

        if( g_androidEnvJavaVM == nullptr )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_GetEnv failed to get JavaVM" );

            return nullptr;
        }

        JNIEnv * new_env;
        jint status = g_androidEnvJavaVM->AttachCurrentThread( &new_env, nullptr );

        if( status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_GetEnv failed to attach current thread [%d]", status );

            return nullptr;
        }

        MengineJNIEnvThread * new_envThread;
        if( Mengine_JNI_SetEnv( new_env, &new_envThread ) == JNI_FALSE )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_GetEnv failed to set environment" );

            g_androidEnvJavaVM->DetachCurrentThread();

        	return nullptr;
        }

        return new_envThread;
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

        if( Mengine_JNI_SetEnv( env, nullptr ) == JNI_FALSE )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetupThread failed to set environment" );

            g_androidEnvJavaVM->DetachCurrentThread();

            return JNI_FALSE;
        }

        __android_log_print( ANDROID_LOG_INFO, "Mengine", "JNI_SetupThread successfully set up thread environment" );

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static JNIEnv * Mengine_JNI_GetEnv( MengineJNIEnvThread * _jenv )
    {
        JNIEnv * env = _jenv->env;

        return env;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_IsInstanceOf( MengineJNIEnvThread * _jenv, jobject _jobject, jclass _jclass )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jboolean result = env->IsInstanceOf( _jobject, _jclass );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassBoolean( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Boolean = _jenv->jclass_Boolean;

        return jclass_Boolean;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassCharacter( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Character = _jenv->jclass_Character;

        return jclass_Character;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassInteger( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Integer = _jenv->jclass_Integer;

        return jclass_Integer;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassLong( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Long = _jenv->jclass_Long;

        return jclass_Long;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassFloat( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Float = _jenv->jclass_Float;

        return jclass_Float;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassDouble( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Double = _jenv->jclass_Double;

        return jclass_Double;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassString( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_String = _jenv->jclass_String;

        return jclass_String;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassException( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Exception = _jenv->jclass_Exception;

        return jclass_Exception;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassList( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_List = _jenv->jclass_List;

        return jclass_List;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassMap( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Map = _jenv->jclass_Map;

        return jclass_Map;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassSet( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Set = _jenv->jclass_Set;

        return jclass_Set;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassArrayList( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_ArrayList = _jenv->jclass_ArrayList;

        return jclass_ArrayList;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassHashMap( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_HashMap = _jenv->jclass_HashMap;

        return jclass_HashMap;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassMapEntry( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_MapEntry = _jenv->jclass_MapEntry;

        return jclass_MapEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassIterator( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Iterator = _jenv->jclass_Iterator;

        return jclass_Iterator;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassRect( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Rect = _jenv->jclass_Rect;

        return jclass_Rect;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassJSONObject( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_JSONObject = _jenv->jclass_JSONObject;

        return jclass_JSONObject;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassJSONArray( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_JSONArray = _jenv->jclass_JSONArray;

        return jclass_JSONArray;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassClass( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_Class = _jenv->jclass_Class;

        return jclass_Class;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassClassLoader( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_ClassLoader = _jenv->jclass_ClassLoader;

        return jclass_ClassLoader;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetObjectClass( MengineJNIEnvThread * _jenv, jobject _jobject )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jclass jclass_Object = env->GetObjectClass( _jobject );

        return jclass_Object;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_DeleteLocalRef( MengineJNIEnvThread * _jenv, jobject _jobject )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        env->DeleteLocalRef( _jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_NewGlobalRef( MengineJNIEnvThread * _jenv, jobject _jobject )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jobjectGlobal = env->NewGlobalRef( _jobject );

        return jobjectGlobal;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_DeleteGlobalRef( MengineJNIEnvThread * _jenv, jobject _jobject )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        env->DeleteGlobalRef( _jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    int Mengine_JNI_ExceptionCheck( MengineJNIEnvThread * _jenv )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        int result = env->ExceptionCheck();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    jthrowable Mengine_JNI_ExceptionOccurred( MengineJNIEnvThread * _jenv )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jthrowable jThrowable = env->ExceptionOccurred();

        return jThrowable;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_ExceptionClear( MengineJNIEnvThread * _jenv )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        env->ExceptionClear();
    }
    //////////////////////////////////////////////////////////////////////////
    int Mengine_JNI_PushLocalFrame( MengineJNIEnvThread * _jenv, jint _capacity )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        int status = env->PushLocalFrame( _capacity );

        return status;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_PopLocalFrame( MengineJNIEnvThread * _jenv, jobject _result )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jobjectResult = env->PopLocalFrame( _result );

        return jobjectResult;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_FindClass( MengineJNIEnvThread * _jenv, const Char * _name )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jclass jclass_FindClass = env->FindClass( _name );

        return jclass_FindClass;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetMethodID( MengineJNIEnvThread * _jenv, jclass _jclass, const Char * _name, const Char * _signature )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jmethodID jmethodID = env->GetMethodID( _jclass, _name, _signature );

        return jmethodID;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetStaticMethodID( MengineJNIEnvThread * _jenv, jclass _jclass, const Char * _name, const Char * _signature )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jmethodID jmethodID = env->GetStaticMethodID( _jclass, _name, _signature );

        return jmethodID;
    }
    //////////////////////////////////////////////////////////////////////////
    jfieldID Mengine_JNI_GetFieldID( MengineJNIEnvThread * _jenv, jclass _jclass, const Char * _name, const Char * _signature )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jfieldID jfieldID = env->GetFieldID( _jclass, _name, _signature );

        return jfieldID;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_GetIntField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jint jintField = env->GetIntField( _jobject, _fieldID );

        return jintField;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_GetBooleanField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jboolean jbooleanField = env->GetBooleanField( _jobject, _fieldID );

        return jbooleanField;
    }
    //////////////////////////////////////////////////////////////////////////
    jchar Mengine_JNI_GetCharField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jchar jcharField = env->GetCharField( _jobject, _fieldID );

        return jcharField;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_GetLongField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jlong jlongField = env->GetLongField( _jobject, _fieldID );

        return jlongField;
    }
    //////////////////////////////////////////////////////////////////////////
    jfloat Mengine_JNI_GetFloatField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jfloat jfloatField = env->GetFloatField( _jobject, _fieldID );

        return jfloatField;
    }
    //////////////////////////////////////////////////////////////////////////
    jdouble Mengine_JNI_GetDoubleField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jdouble jdoubleField = env->GetDoubleField( _jobject, _fieldID );

        return jdoubleField;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jobjectField = env->GetObjectField( _jobject, _fieldID );

        return jobjectField;
    }
    //////////////////////////////////////////////////////////////////////////
    jfieldID Mengine_JNI_GetStaticFieldID( MengineJNIEnvThread * _jenv, jclass _jclass, const Char * _name, const Char * _signature )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jfieldID jfieldID = env->GetStaticFieldID( _jclass, _name, _signature );

        return jfieldID;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetStaticObjectField( MengineJNIEnvThread * _jenv, jclass _jclass, jfieldID _fieldID )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jobjectField = env->GetStaticObjectField( _jclass, _fieldID );

        return jobjectField;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallVoidMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        Mengine_JNI_CallVoidMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallVoidMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        env->CallVoidMethodV( _jobject, _methodID, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallVoidMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        env->CallVoidMethodA( _jobject, _methodID, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallBooleanMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jboolean jboolean_result = Mengine_JNI_CallBooleanMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallBooleanMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jboolean jboolean_result = env->CallBooleanMethodV( _jobject, _methodID, _args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallBooleanMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jboolean jboolean_result = env->CallBooleanMethodA( _jobject, _methodID, _args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jchar Mengine_JNI_CallCharMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jchar jchar_result = Mengine_JNI_CallCharMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jchar_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jchar Mengine_JNI_CallCharMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jchar jchar_result = env->CallCharMethodV( _jobject, _methodID, _args );

        return jchar_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jchar Mengine_JNI_CallCharMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jchar jchar_result = env->CallCharMethodA( _jobject, _methodID, _args );

        return jchar_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallIntMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jint jint_result = Mengine_JNI_CallIntMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallIntMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jint jint_result = env->CallIntMethodV( _jobject, _methodID, _args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallIntMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jint jint_result = env->CallIntMethodA( _jobject, _methodID, _args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallLongMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jlong jlong_result = Mengine_JNI_CallLongMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallLongMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jlong jlong_result = env->CallLongMethodV( _jobject, _methodID, _args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallLongMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jlong jlong_result = env->CallLongMethodA( _jobject, _methodID, _args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jfloat Mengine_JNI_CallFloatMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jfloat jfloat_result = Mengine_JNI_CallFloatMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jfloat_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jfloat Mengine_JNI_CallFloatMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jfloat jfloat_result = env->CallFloatMethodV( _jobject, _methodID, _args );

        return jfloat_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jfloat Mengine_JNI_CallFloatMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jfloat jfloat_result = env->CallFloatMethodA( _jobject, _methodID, _args );

        return jfloat_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jdouble Mengine_JNI_CallDoubleMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jdouble jdouble_result = Mengine_JNI_CallDoubleMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jdouble_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jdouble Mengine_JNI_CallDoubleMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jdouble jdouble_result = env->CallDoubleMethodV( _jobject, _methodID, _args );

        return jdouble_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jdouble Mengine_JNI_CallDoubleMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jdouble jdouble_result = env->CallDoubleMethodA( _jobject, _methodID, _args );

        return jdouble_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallObjectMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jobject jobject_result = Mengine_JNI_CallObjectMethodV( _jenv, _jobject, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallObjectMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jobject_result = env->CallObjectMethodV( _jobject, _methodID, _args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallObjectMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jobject_result = env->CallObjectMethodA( _jobject, _methodID, _args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallStaticVoidMethod( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        Mengine_JNI_CallStaticVoidMethodV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallStaticVoidMethodV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        env->CallStaticVoidMethodV( _jclass, _methodID, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_CallStaticVoidMethodA( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        env->CallStaticVoidMethodA( _jclass, _methodID, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallStaticIntMethod( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jint jint_result = Mengine_JNI_CallStaticIntMethodV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallStaticIntMethodV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jint jint_result = env->CallStaticIntMethodV( _jclass, _methodID, _args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jint Mengine_JNI_CallStaticIntMethodA( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jint jint_result = env->CallStaticIntMethodA( _jclass, _methodID, _args );

        return jint_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallStaticLongMethod( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jlong jlong_result = Mengine_JNI_CallStaticLongMethodV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallStaticLongMethodV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jlong jlong_result = env->CallStaticLongMethodV( _jclass, _methodID, _args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong Mengine_JNI_CallStaticLongMethodA( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jlong jlong_result = env->CallStaticLongMethodA( _jclass, _methodID, _args );

        return jlong_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallStaticBooleanMethod( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jboolean jboolean_result = Mengine_JNI_CallStaticBooleanMethodV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallStaticObjectMethod( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jobject jobject_result = Mengine_JNI_CallStaticObjectMethodV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallStaticBooleanMethodV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jboolean jboolean_result = env->CallStaticBooleanMethodV( _jclass, _methodID, _args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_CallStaticBooleanMethodA( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jboolean jboolean_result = env->CallStaticBooleanMethodA( _jclass, _methodID, _args );

        return jboolean_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallStaticObjectMethodV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jobject_result = env->CallStaticObjectMethodV( _jclass, _methodID, _args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_CallStaticObjectMethodA( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jobject_result = env->CallStaticObjectMethodA( _jclass, _methodID, _args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_NewObject( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _methodID );

        jobject jobject_result = Mengine_JNI_NewObjectV( _jenv, _jclass, _methodID, args );

        MENGINE_VA_LIST_END( args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_NewObjectV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jobject_result = env->NewObjectV( _jclass, _methodID, _args );

        return jobject_result;
    }
    //////////////////////////////////////////////////////////////////////////
    jstring Mengine_JNI_NewStringUTF( MengineJNIEnvThread * _jenv, const Char * _str )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jstring jstr = env->NewStringUTF( _str );

        return jstr;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Mengine_JNI_GetStringUTFChars( MengineJNIEnvThread * _jenv, jstring _jstring, jboolean * const _isCopy )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        const Char * str = env->GetStringUTFChars( _jstring, _isCopy );

        return str;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_ReleaseStringUTFChars( MengineJNIEnvThread * _jenv, jstring _jstring, const Char * _cstr )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        env->ReleaseStringUTFChars( _jstring, _cstr );
    }
    //////////////////////////////////////////////////////////////////////////
    jsize Mengine_JNI_GetStringLength( MengineJNIEnvThread * _jenv, jstring _jstring )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jsize length = env->GetStringLength( _jstring );

        return length;
    }
    //////////////////////////////////////////////////////////////////////////
    jsize Mengine_JNI_GetArrayLength( MengineJNIEnvThread * _jenv, jobjectArray _jarray )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jsize length = env->GetArrayLength( _jarray );

        return length;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectArrayElement( MengineJNIEnvThread * _jenv, jobjectArray _jarray, jsize _index )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jobject_element = env->GetObjectArrayElement( _jarray, _index );

        return jobject_element;
    }
    //////////////////////////////////////////////////////////////////////////
    jbyteArray Mengine_JNI_NewByteArray( MengineJNIEnvThread * _jenv, jsize _length )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jbyteArray jbyte_array = env->NewByteArray( _length );

        return jbyte_array;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_SetByteArrayRegion( MengineJNIEnvThread * _jenv, jbyteArray _jarray, jsize _start, jsize _len, const jbyte * _buf )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        env->SetByteArrayRegion( _jarray, _start, _len, _buf );
    }
    //////////////////////////////////////////////////////////////////////////
    void Mengine_JNI_ReleaseByteArrayElements( MengineJNIEnvThread * _jenv, jbyteArray _jarray, jbyte * _elements, jint _mode )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        env->ReleaseByteArrayElements( _jarray, _elements, _mode );
    }
    //////////////////////////////////////////////////////////////////////////
    jbyte * Mengine_JNI_GetByteArrayElements( MengineJNIEnvThread * _jenv, jbyteArray _jarray, jboolean * const _isCopy )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jbyte * byte_elements = env->GetByteArrayElements( _jarray, _isCopy );

        return byte_elements;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_NewDirectByteBuffer( MengineJNIEnvThread * _jenv, void * _address, jlong _capacity )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        jobject jbyte_buffer = env->NewDirectByteBuffer( _address, _capacity );

        return jbyte_buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void * Mengine_JNI_GetDirectBufferAddress( MengineJNIEnvThread * _jenv, jobject _jbuffer )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        void * address = env->GetDirectBufferAddress( _jbuffer );

        return address;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassMengineCallback( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_MengineCallback = _jenv->jclass_MengineCallback;

        return jclass_MengineCallback;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassApplication( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_MengineApplication = _jenv->jclass_MengineApplication;

        return jclass_MengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectApplication( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_MengineApplication = Mengine_JNI_GetClassApplication( _jenv );

        jfieldID jfield_INSTANCE = Mengine_JNI_GetStaticFieldID( _jenv, jclass_MengineApplication, "INSTANCE", "Lorg/Mengine/Base/MengineApplication;" );

        if( jfield_INSTANCE == nullptr )
        {
            return nullptr;
        }

        jobject jobject_MengineApplication = Mengine_JNI_GetStaticObjectField( _jenv, jclass_MengineApplication, jfield_INSTANCE );

        return jobject_MengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetMethodApplication( MengineJNIEnvThread * _jenv, const char * _methodName, const char * _signature )
    {
        jclass jclass_MengineApplication = Mengine_JNI_GetClassApplication( _jenv );

        jmethodID jmethod_MengineApplication = Mengine_JNI_GetMethodID( _jenv, jclass_MengineApplication, _methodName, _signature );

        return jmethod_MengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassActivity( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_MengineActivity = _jenv->jclass_MengineActivity;

        return jclass_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectActivity( MengineJNIEnvThread * _jenv )
    {
        jclass jclass_MengineActivity = Mengine_JNI_GetClassActivity( _jenv );

        jfieldID jfield_INSTANCE = Mengine_JNI_GetStaticFieldID( _jenv, jclass_MengineActivity, "INSTANCE", "Lorg/Mengine/Base/MengineActivity;" );

        if( jfield_INSTANCE == nullptr )
        {
            return nullptr;
        }

        jobject jobject_MengineActivity = Mengine_JNI_GetStaticObjectField( _jenv,  jclass_MengineActivity, jfield_INSTANCE );

        return jobject_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetMethodActivity( MengineJNIEnvThread * _jenv, const Char * _methodName, const Char * _signature )
    {
        jclass jclass_MengineActivity = Mengine_JNI_GetClassActivity( _jenv );

        jmethodID jmethodId_MengineActivity = Mengine_JNI_GetMethodID( _jenv, jclass_MengineActivity, _methodName, _signature );

        return jmethodId_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassFragment( MengineJNIEnvThread * _jenv, const Char * _fragmentName )
    {
        Char fragmentClassName[256 + 1] = { '\0' };
        StdString::strcpy_safe( fragmentClassName, "org/Mengine/Base/", 256 );
        StdString::strcat_safe( fragmentClassName, _fragmentName, 256 );

        jclass jclass_FragmentClassName = Mengine_JNI_LoadClass( _jenv, fragmentClassName );

        return jclass_FragmentClassName;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectFragment( MengineJNIEnvThread * _jenv, const Char * _fragmentName )
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
    jmethodID Mengine_JNI_GetMethodFragment( MengineJNIEnvThread * _jenv, const Char * _fragment, const Char * _method, const Char * _signature )
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
    jclass Mengine_JNI_LoadClass( MengineJNIEnvThread * _jenv, const Char * _className )
    {
        jclass jclass_ClassLoader = Mengine_JNI_GetClassClassLoader( _jenv );
        jmethodID jmethodID_ClassLoader_loadClass = Mengine_JNI_GetMethodID( _jenv, jclass_ClassLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;" );

        jstring jstring_className = Mengine_JNI_NewStringUTF( _jenv, _className );
        jclass jclass_FindClass = (jclass)Mengine_JNI_CallObjectMethod( _jenv, g_jobject_MengineClassLoader, jmethodID_ClassLoader_loadClass, jstring_className );
        Mengine_JNI_DeleteLocalRef( _jenv, jstring_className );

        return jclass_FindClass;
    }
    //////////////////////////////////////////////////////////////////////////
    AAssetManager * Mengine_JNI_GetAssetManagerFromJava( MengineJNIEnvThread * _jenv, jobject _jassetManager )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        AAssetManager * assetManager = AAssetManager_fromJava( env, _jassetManager );

        return assetManager;
    }
    //////////////////////////////////////////////////////////////////////////
    ANativeWindow * Mengine_JNI_ANativeWindow_fromSurface( MengineJNIEnvThread * _jenv, jobject _surface )
    {
        JNIEnv * env = Mengine_JNI_GetEnv( _jenv );

        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( env, _surface );

        return nativeWindow;
    }
    //////////////////////////////////////////////////////////////////////////
}