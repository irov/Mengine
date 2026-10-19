#include "AndroidWebViewApplication.h"

#include "Environment/Android/AndroidDeclaration.h"
#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidPluginHelper.h"

#include <unistd.h>

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jobject JNICALL MENGINE_JAVA_INTERFACE( AndroidMain_1bootstrap )( JNIEnv * _env, jclass, jstring, jobjectArray )
    {
        Mengine::AndroidWebViewApplication * application = new Mengine::AndroidWebViewApplication();

        if( application->bootstrap( _env ) == false )
        {
            application->finalize();
            delete application;

            return nullptr;
        }

        jobject buffer = Mengine::Mengine_JNI_NewDirectByteBuffer( _env, application );

        if( buffer == nullptr )
        {
            application->finalize();
            delete application;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_JAVA_INTERFACE( AndroidMain_1main )( JNIEnv * _env, jclass, jobject _buffer )
    {
        Mengine::AndroidWebViewApplication * application = Mengine::Mengine_JNI_GetDirectBufferAddress<Mengine::AndroidWebViewApplication *>( _env, _buffer );
        bool result = application->run();

        if( result == false )
        {
            Mengine::Helper::AndroidCallVoidPluginMethod( _env, "AndroidWebViewPlugin", "onWebViewRuntimeFailed", "()V" );
            useconds_t quitPollInterval = 10000;

            while( true )
            {
                bool quitting = Mengine::Helper::AndroidCallBooleanPluginMethod( _env, "AndroidWebViewPlugin", "isWebViewQuitRequested", "()Z" );

                if( quitting == true )
                {
                    break;
                }

                ::usleep( quitPollInterval );
            }
        }

        application->finalize();

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidMain_1destroy )( JNIEnv * _env, jclass, jobject _buffer )
    {
        Mengine::AndroidWebViewApplication * application = Mengine::Mengine_JNI_GetDirectBufferAddress<Mengine::AndroidWebViewApplication *>( _env, _buffer );
        delete application;
    }
    //////////////////////////////////////////////////////////////////////////
}
