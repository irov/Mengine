#include "SDLMessageBoxLogger.h"

#include "SDL_messagebox.h"

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
    void SDLMessageBoxLogger::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _color );
        MENGINE_UNUSED( _size );

        printf( "%.*s", (int32_t)_size, _data );

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
    //////////////////////////////////////////////////////////////////////////
}
