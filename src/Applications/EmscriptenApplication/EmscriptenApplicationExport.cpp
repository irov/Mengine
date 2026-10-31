#include "Interface/ApplicationInterface.h"

#include "Environment/Emscripten/EmscriptenIncluder.h"

//////////////////////////////////////////////////////////////////////////
extern "C" EMSCRIPTEN_KEEPALIVE void mengineStop()
{
    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    if( SERVICE_IS_INITIALIZE( Mengine::ApplicationInterface ) == false )
    {
        return;
    }

    APPLICATION_SERVICE()
        ->quit();
}
//////////////////////////////////////////////////////////////////////////
