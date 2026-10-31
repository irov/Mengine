#include "EmscriptenApplication.h"

#include "Environment/Emscripten/EmscriptenIncluder.h"

#include "Kernel/Configuration.h"

//////////////////////////////////////////////////////////////////////////
int main( int _argc, char * _argv[] )
{
    Mengine::EmscriptenApplication application;

    Mengine::Configuration configuration;
    configuration.silentDialog = true;

    if( application.bootstrap( _argc, _argv, configuration ) == false )
    {
        application.finalize();

        emscripten_log( EM_LOG_ERROR, "Engine bootstrap failed; see log." );

        return 1;
    }

    if( application.initialize() == false )
    {
        application.finalize();

        emscripten_log( EM_LOG_ERROR, "Engine initialization failed; see log." );

        return 1;
    }

    application.loop();

    return 0;
}
//////////////////////////////////////////////////////////////////////////
