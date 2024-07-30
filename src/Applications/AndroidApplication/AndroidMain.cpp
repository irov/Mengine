#define SDL_MAIN_HANDLED

#include "Environment/Android/AndroidDeclaration.h"

#include "AndroidApplication.h"

JNIEXPORT int JNICALL MENGINE_MAIN_JAVA_INTERFACE( AndroidMain_1nativeRunMain )(JNIEnv * _env, jclass _cls, jobjectArray _options)
{
    Mengine::AndroidApplication application;

    if( application.bootstrap( 0, nullptr ) == false )
    {
        application.finalize();

        return EXIT_FAILURE;
    }

    if( application.initialize() == false )
    {
        application.finalize();

        return EXIT_FAILURE;
    }

    application.loop();

    application.finalize();

    return EXIT_SUCCESS;
}