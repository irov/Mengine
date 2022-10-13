#include "SDLMessageBoxLogger.h"

#include "Kernel/LoggerHelper.h"

#include "Config/StdIO.h"

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
    void SDLMessageBoxLogger::log( const LoggerMessage & _message )
    {
        Char timestamp[256] = {'\0'};
        Helper::makeLoggerTimestamp( _message.dateTime, "[%02u:%02u:%02u:%04u] ", timestamp, 256 );
        ::printf( "%s "
            , timestamp
        );

        if( _message.category.empty() == false )
        {
            ::printf( "%s "
                , _message.category.c_str()
            );
        }

        const Char * data = _message.data;
        size_t size = _message.size;

        ::printf( "%s %.*s\n", timestamp, (int32_t)size, data );

        if( _message.level == LM_CRITICAL )
        {
            Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'0'};
            MENGINE_SNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, "%.*s"
                , (int32_t)size
                , data
            );

            SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine critical error", message, NULL );
        }
        else if( _message.level == LM_FATAL )
        {
            Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'0'};
            MENGINE_SNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, "%.*s"
                , (int32_t)size
                , data
            );

            SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine fatal error", message, NULL );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
