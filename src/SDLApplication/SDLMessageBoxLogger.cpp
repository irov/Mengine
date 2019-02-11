#include "SDLMessageBoxLogger.h"

#include "Config/Config.h"

#include "SDL_messagebox.h"

#include <cstdio>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLMessageBoxLogger::SDLMessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLMessageBoxLogger::~SDLMessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLMessageBoxLogger::log( EMessageLevel _level, uint32_t _flag, const Char * _data, uint32_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _size );

        printf( "%s", _data );

        if( _level == LM_CRITICAL )
        {
            SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine critical error", _data, NULL );
        }
        else if( _level == LM_FATAL )
        {
            SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine fatal error", _data, NULL );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLMessageBoxLogger::flush()
    {
        //fflush(stdout);
    }
}
