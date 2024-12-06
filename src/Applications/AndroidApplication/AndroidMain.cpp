#include "Environment/Android/AndroidDeclaration.h"
#include "Environment/Android/AndroidEnv.h"

#include "AndroidApplication.h"

#include "Kernel/Crash.h"

#include "Config/StdLib.h"
#include "Config/New.h"

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    static bool already_bootstrapped = false;
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jobject JNICALL MENGINE_JAVA_INTERFACE( AndroidMain_1bootstrap )(JNIEnv * _env, jclass _cls, jstring _nativeLibraryDir, jobjectArray _args)
    {
        if( already_bootstrapped == true )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "Android bootstrap already bootstrapped" );

            return nullptr;
        }
        
        already_bootstrapped = true;

        if( Mengine::Mengine_JNI_Initialize( _env ) == JNI_FALSE )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "Android bootstrap JNI initialize failed" );

            return nullptr;
        }

        Mengine::AndroidApplication * application = new Mengine::AndroidApplication();

        const Mengine::Char * nativeLibraryDir_str = _env->GetStringUTFChars( _nativeLibraryDir, nullptr );

        jsize argc = _env->GetArrayLength( _args );

        if( argc >= 256 )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "Android bootstrap too many arguments" );

            delete application;

            return nullptr;
        }

        Mengine::Char * argv[256];

        for( jsize i = 0; i != argc; ++i )
        {
            jstring j_arg = (jstring)_env->GetObjectArrayElement( _args, i );

            const Mengine::Char * arg_str = _env->GetStringUTFChars( j_arg, nullptr );

            argv[i] = (Mengine::Char *)arg_str;
        }

        bool result = application->bootstrap( nativeLibraryDir_str, argc, argv );

        for( jsize i = 0; i != argc; ++i )
        {
            jstring j_arg = (jstring)_env->GetObjectArrayElement( _args, i );

            _env->ReleaseStringUTFChars( j_arg, argv[i] );
        }

        _env->ReleaseStringUTFChars( _nativeLibraryDir, nativeLibraryDir_str );

        if( result == false )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "Android bootstrap failed" );

            delete application;

            return nullptr;
        }

        jobject j_application = _env->NewDirectByteBuffer( application, sizeof(void *) );

        if ( j_application == nullptr)
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "Android bootstrap failed to create direct buffer" );

            delete application;

            return nullptr;
        }

        return j_application;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_JAVA_INTERFACE( AndroidMain_1main )(JNIEnv * _env, jclass _cls, jobject _application)
    {
        if( Mengine::Mengine_JNI_SetupThread() == JNI_FALSE )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "Android main JNI setup thread failed" );

            return false;
        }

        Mengine::AndroidApplication * application = (Mengine::AndroidApplication *)_env->GetDirectBufferAddress( _application );

        if( application->initialize() == false )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "Android main initialize failed" );

            return false;
        }

        application->loop();

        application->finalize();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidMain_1destroy )(JNIEnv * _env, jclass _cls, jobject _application)
    {
        Mengine::AndroidApplication * application = (Mengine::AndroidApplication *)_env->GetDirectBufferAddress( _application );

        delete application;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidMain_1crash )(JNIEnv * _env, jclass _cls, jstring _reason)
    {
        const Mengine::Char * reason_str = _env->GetStringUTFChars( _reason, nullptr );

        Mengine::Helper::crash( reason_str );

        _env->ReleaseStringUTFChars( _reason, reason_str );
    }
    //////////////////////////////////////////////////////////////////////////
}