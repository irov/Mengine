#include "SDL2MessageBoxLogger.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/DebugBreak.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL2MessageBoxLogger::SDL2MessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2MessageBoxLogger::~SDL2MessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2MessageBoxLogger::_log( const LoggerRecordInterfacePtr & _record )
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
