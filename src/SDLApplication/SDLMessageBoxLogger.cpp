#include "SDLMessageBoxLogger.h"

#include "Environment/SDL/SDLIncluder.h"

#include "Kernel/DebugBreak.h"

#include "Config/StdIO.h"

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
    void SDLMessageBoxLogger::_log( const LoggerMessage & _message )
    {
        const Char * data = _message.data;
        size_t size = _message.size;

        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        MENGINE_SNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, "%.*s"
            , (int32_t)size
            , data
        );

        Helper::debuggerBreak();

        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine fatal error", message, NULL );
    }
    //////////////////////////////////////////////////////////////////////////
}
