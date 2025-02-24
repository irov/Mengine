#include "SDLMessageBoxLogger.h"

#include "Environment/SDL/SDLIncluder.h"

#include "Kernel/DebugBreak.h"

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

#if defined(MENGINE_DEBUG)
        Helper::debuggerBreak();
#endif

        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine fatal error", message.data, NULL );
    }
    //////////////////////////////////////////////////////////////////////////
}
