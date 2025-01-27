#include "AndroidEnv.h"

#include "AndroidDeclaration.h"

#include "Kernel/BuildMode.h"
#include "Kernel/Assertion.h"

#include <signal.h>
#include <pthread.h>

static JavaVM * g_androidEnvJavaVM;

static jobject g_jobject_MengineApplication;
static jobject g_jobject_MengineClassLoader;
static jobject g_jobject_MengineActivity;

extern "C" 
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1nativeDebugBreak )( JNIEnv * env, jclass cls )
    {
        jclass jclassMengineUtils = env->FindClass("org/Mengine/Base/MengineUtils");
        jmethodID jmethodIdPrintCurrentStackTrace = env->GetStaticMethodID(jclassMengineUtils, "printCurrentStackTrace", "()V");

        env->CallStaticVoidMethod(jclassMengineUtils, jmethodIdPrintCurrentStackTrace);

        ::raise( SIGTRAP );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1setMengineAndroidApplicationJNI )( JNIEnv * env, jclass cls, jobject obj, jobject cl )
    {
        g_jobject_MengineApplication = (jobject)env->NewGlobalRef( obj );
        g_jobject_MengineClassLoader = (jobject)env->NewGlobalRef( cl );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1removeMengineAndroidApplicationJNI )( JNIEnv * env, jclass cls )
    {
        env->DeleteGlobalRef( g_jobject_MengineApplication );
        g_jobject_MengineApplication = nullptr;

        env->DeleteLocalRef( g_jobject_MengineClassLoader );
        g_jobject_MengineClassLoader = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1setMengineAndroidActivityJNI )( JNIEnv * env, jclass cls, jobject obj )
    {
        g_jobject_MengineActivity = (jobject)env->NewGlobalRef( obj );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1removeMengineAndroidActivityJNI )( JNIEnv * env, jclass cls )
    {
        env->DeleteGlobalRef( g_jobject_MengineActivity );
        g_jobject_MengineActivity = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1isMasterRelease )( JNIEnv * env, jclass cls )
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

        JNIEnv *env;
        int get_status = g_androidEnvJavaVM->GetEnv( (void **)&env, JNI_VERSION_1_6 );

        if( get_status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_OnLoad failed to get JNI Env" );

            return JNI_ERR;
        }

        return JNI_VERSION_1_6;
    }
    //////////////////////////////////////////////////////////////////////////
}

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_ExistMengineApplication()
    {
        if( g_jobject_MengineApplication == nullptr )
        {
            return JNI_FALSE;
        }

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetJClassMengineApplication( JNIEnv * _jenv )
    {
        jclass jclass_MengineApplication = _jenv->GetObjectClass( g_jobject_MengineApplication );

        return jclass_MengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetJObjectMengineApplication()
    {
        return g_jobject_MengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean Mengine_JNI_ExistMengineActivity()
    {
        if( g_jobject_MengineActivity == nullptr )
        {
            return JNI_FALSE;
        }

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetJClassMengineActivity( JNIEnv * _jenv )
    {
        jclass jclass_MengineActivity = _jenv->GetObjectClass( g_jobject_MengineActivity );

        return jclass_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetJObjectMengineActivity()
    {
        return g_jobject_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    static pthread_key_t g_androidEnvThreadKey = 0;
    //////////////////////////////////////////////////////////////////////////
    static void Mengine_JNI_ThreadDestroyed( void * _value )
    {
        JNIEnv * env = (JNIEnv *)_value;

        if( env == nullptr )
        {
            return;
        }

        g_androidEnvJavaVM->DetachCurrentThread();

        ::pthread_setspecific( g_androidEnvThreadKey, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    static int Mengine_JNI_SetEnv( JNIEnv * _env )
    {
        MENGINE_ASSERTION_FATAL( g_androidEnvThreadKey != 0, "android ENV thread key not initialized" );
        MENGINE_ASSERTION_FATAL( pthread_getspecific( g_androidEnvThreadKey ) == nullptr, "ENV thread key already set" );

        int status = ::pthread_setspecific( g_androidEnvThreadKey, (void *)_env );

        if( status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetEnv failed to set pthread key" );

            return JNI_FALSE;
        }

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    int Mengine_JNI_Initialize( JNIEnv * _env )
    {
        MENGINE_ASSERTION_FATAL( g_androidEnvThreadKey == 0, "android ENV thread key already initialize" );

        int status = ::pthread_key_create( &g_androidEnvThreadKey, &Mengine_JNI_ThreadDestroyed );

        if( status < JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetEnv failed to create pthread key" );

            return JNI_FALSE;
        }

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

        JNIEnv * env = (JNIEnv *)::pthread_getspecific( g_androidEnvThreadKey );

        if( env != nullptr )
        {
            return env;
        }

        if( g_androidEnvJavaVM == nullptr )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_GetEnv failed to get JavaVM" );

            return nullptr;
        }

        JNIEnv * new_env;
        jint attach_status = g_androidEnvJavaVM->AttachCurrentThread( &new_env, nullptr );

        if( attach_status != JNI_OK )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_GetEnv failed to attach current thread" );

            return nullptr;
        }

        if( Mengine_JNI_SetEnv( new_env ) == JNI_FALSE )
        {
        	return nullptr;
        }

        return new_env;
    }
    //////////////////////////////////////////////////////////////////////////
    int Mengine_JNI_SetupThread()
    {
        if( g_androidEnvJavaVM == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetupThread failed to get JavaVM");

            return JNI_FALSE;
        }

        JNIEnv * env;
        jint attach_status = g_androidEnvJavaVM->AttachCurrentThread( &env, nullptr );

        if( attach_status != JNI_OK )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "[ERROR] JNI_SetupThread failed to attach current thread");

            return JNI_FALSE;
        }

        if( Mengine_JNI_SetEnv( env ) == JNI_FALSE )
        {
            return JNI_FALSE;
        }

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_FindClass( JNIEnv * _jenv, const char * _className )
    {
        jclass jclass_ClassLoader = _jenv->FindClass( "java/lang/ClassLoader" );
        jmethodID jmethodID_ClassLoader_loadClass = _jenv->GetMethodID( jclass_ClassLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;" );
        _jenv->DeleteLocalRef( jclass_ClassLoader );

        jstring jstring_className = _jenv->NewStringUTF( _className );
        jclass jclass_FindClass = (jclass)_jenv->CallObjectMethod( g_jobject_MengineClassLoader, jmethodID_ClassLoader_loadClass, jstring_className );
        _jenv->DeleteLocalRef( jstring_className );

        return jclass_FindClass;
    }
    //////////////////////////////////////////////////////////////////////////
}