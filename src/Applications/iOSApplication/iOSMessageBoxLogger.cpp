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
    void SDLMessageBoxLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        Helper::debuggerBreak();

        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine fatal error", message.data, NULL );
    }
    //////////////////////////////////////////////////////////////////////////
}
