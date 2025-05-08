#include "AndroidEnv.h"

#include "AndroidDeclaration.h"

#include "Kernel/BuildMode.h"
#include "Kernel/Assertion.h"

#include "Config/StdString.h"

#include <signal.h>
#include <pthread.h>

static JavaVM * g_androidEnvJavaVM;

static jobject g_jobject_MengineClassLoader;

extern "C" 
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1nativeDebugBreak )( JNIEnv * env, jclass cls )
    {
        jclass jclassMengineUtils = Mengine::Mengine_JNI_FindClass( env, "org/Mengine/Base/MengineUtils" );

        jmethodID jmethodIdPrintCurrentStackTrace = env->GetStaticMethodID( jclassMengineUtils, "printCurrentStackTrace", "()V" );

        env->CallStaticVoidMethod( jclassMengineUtils, jmethodIdPrintCurrentStackTrace );

        env->DeleteLocalRef( jclassMengineUtils );

        ::raise( SIGTRAP );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1setMengineAndroidClassLoaderJNI )( JNIEnv * env, jclass cls, jobject cl )
    {
        g_jobject_MengineClassLoader = (jobject)env->NewGlobalRef( cl );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnv_1removeMengineAndroidClassLoaderJNI )( JNIEnv * env, jclass cls )
    {
        env->DeleteLocalRef( g_jobject_MengineClassLoader );
        g_jobject_MengineClassLoader = nullptr;
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
    jclass Mengine_JNI_GetClassApplication( JNIEnv * _jenv )
    {
        jclass jclassMengineApplication = Mengine::Mengine_JNI_FindClass( _jenv, "org/Mengine/Base/MengineApplication" );

        return jclassMengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectApplication( JNIEnv * _jenv )
    {
        jclass jclassMengineApplication = Mengine_JNI_GetClassApplication( _jenv );

        if( jclassMengineApplication == nullptr )
        {
            return nullptr;
        }

        jfieldID jfieldINSTANCE = _jenv->GetStaticFieldID( jclassMengineApplication, "INSTANCE", "Lorg/Mengine/Base/MengineApplication;" );

        if( jfieldINSTANCE == nullptr )
        {
            _jenv->DeleteLocalRef( jclassMengineApplication );

            return nullptr;
        }

        jobject jobjectMengineApplication = _jenv->GetStaticObjectField( jclassMengineApplication, jfieldINSTANCE );

        _jenv->DeleteLocalRef( jclassMengineApplication );

        return jobjectMengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetMethodApplication( JNIEnv * _jenv, const char * _methodName, const char * _signature )
    {
        jclass jclassMengineApplication = Mengine_JNI_GetClassApplication( _jenv );

        if( jclassMengineApplication == nullptr )
        {
            return nullptr;
        }

        jmethodID midMengineApplication = _jenv->GetMethodID( jclassMengineApplication, _methodName, _signature );

        _jenv->DeleteLocalRef( jclassMengineApplication );

        return midMengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassActivity( JNIEnv * _jenv )
    {
        jclass jclassMengineActivity = Mengine::Mengine_JNI_FindClass( _jenv, "org/Mengine/Base/MengineActivity" );

        return jclassMengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectActivity( JNIEnv * _jenv )
    {
        jclass jclassMengineActivity = Mengine_JNI_GetClassActivity( _jenv );

        if( jclassMengineActivity == nullptr )
        {
            return nullptr;
        }

        jfieldID jfieldINSTANCE = _jenv->GetStaticFieldID( jclassMengineActivity, "INSTANCE", "Lorg/Mengine/Base/MengineActivity;" );

        if( jfieldINSTANCE == nullptr )
        {
            _jenv->DeleteLocalRef( jclassMengineActivity );

            return nullptr;
        }

        jobject jobjectMengineActivity = _jenv->GetStaticObjectField( jclassMengineActivity, jfieldINSTANCE );

        _jenv->DeleteLocalRef( jclassMengineActivity );

        return jobjectMengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetMethodActivity( JNIEnv * _jenv, const Char * _methodName, const Char * _signature )
    {
        jclass jclassMengineActivity = Mengine_JNI_GetClassActivity( _jenv );

        if( jclassMengineActivity == nullptr )
        {
            return nullptr;
        }

        jmethodID midMengineActivity = _jenv->GetMethodID( jclassMengineActivity, _methodName, _signature );

        _jenv->DeleteLocalRef( jclassMengineActivity );

        return midMengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetClassFragment( JNIEnv * _jenv, const Char * _fragmentName )
    {
        Char fragmentClassName[256] = { '\0' };
        StdString::strcpy_safe( fragmentClassName, "org/Mengine/Base/", 256 );
        StdString::strcat_safe( fragmentClassName, _fragmentName, 256 );

        jclass fragmentClass = Mengine::Mengine_JNI_FindClass( _jenv, fragmentClassName );

        return fragmentClass;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetObjectFragment( JNIEnv * _jenv, const Char * _fragmentName )
    {
        jclass fragmentClass = Mengine::Mengine_JNI_GetClassFragment( _jenv, _fragmentName );

        if( fragmentClass == nullptr )
        {
            return nullptr;
        }

        Char fragmentSignature[256] = { '\0' };
        StdString::strcpy_safe( fragmentSignature, "Lorg/Mengine/Base/", 256 );
        StdString::strcat_safe( fragmentSignature, _fragmentName, 256 );
        StdString::strcat_safe( fragmentSignature, ";", 256 );

        jfieldID instanceField = _jenv->GetStaticFieldID( fragmentClass, "INSTANCE", fragmentSignature );

        if( instanceField == nullptr )
        {
            _jenv->DeleteLocalRef( fragmentClass );

            return nullptr;
        }

        jobject applicationInstance = _jenv->GetStaticObjectField( fragmentClass, instanceField );

        _jenv->DeleteLocalRef( fragmentClass );

        return applicationInstance;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID Mengine_JNI_GetMethodFragment( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature )
    {
        jclass jclass_Fragment = Mengine::Mengine_JNI_GetClassFragment( _jenv, _fragment );

        if( jclass_Fragment == nullptr )
        {
            return nullptr;
        }

        jmethodID methodId = _jenv->GetMethodID( jclass_Fragment, _method, _signature );

        _jenv->DeleteLocalRef( jclass_Fragment );

        return methodId;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_FindClass( JNIEnv * _jenv, const Char * _className )
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