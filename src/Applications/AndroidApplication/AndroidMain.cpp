#include "Environment/Android/AndroidDeclaration.h"

#include "AndroidApplication.h"

#include "Config/StdLib.h"
#include "Config/New.h"

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jobject JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidMain_1bootstrap )(JNIEnv * _env, jclass _cls, jstring _nativeLibraryDir, jobjectArray _args)
    {
        size_t size_AndroidApplication = sizeof( Mengine::AndroidApplication );

        void * memory_application = Mengine::StdLib::malloc( size_AndroidApplication );

        if( memory_application == nullptr )
        {
            return nullptr;
        }

        Mengine::AndroidApplication * application = new (memory_application) Mengine::AndroidApplication();

        const Mengine::Char * nativeLibraryDir_str = _env->GetStringUTFChars( _nativeLibraryDir, nullptr );

        jsize argc = _env->GetArrayLength( _args );

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
            application->~AndroidApplication();

            Mengine::StdLib::free( memory_application );

            return nullptr;
        }

        jobject j_application = _env->NewDirectByteBuffer( memory_application, size_AndroidApplication );

        return j_application;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_MAIN_JAVA_INTERFACE( AndroidMain_1main )(JNIEnv * _env, jclass _cls, jobject _application)
    {
        Mengine::AndroidApplication * application = (Mengine::AndroidApplication *)_env->GetDirectBufferAddress( _application );

        if( application->initialize() == false )
        {
            return false;
        }

        application->loop();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidMain_1destroy )(JNIEnv * _env, jclass _cls, jobject _application)
    {
        Mengine::AndroidApplication * application = (Mengine::AndroidApplication *)_env->GetDirectBufferAddress( _application );

        application->finalize();

        application->~AndroidApplication();

        Mengine::StdLib::free( application );
    }
    //////////////////////////////////////////////////////////////////////////
}