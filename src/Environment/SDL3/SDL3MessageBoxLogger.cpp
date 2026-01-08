#include "SDL3MessageBoxLogger.h"

#include "Environment/SDL3/SDL3Includer.h"

#include "Kernel/DebugBreak.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3MessageBoxLogger::SDL3MessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3MessageBoxLogger::~SDL3MessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3MessageBoxLogger::_log( const LoggerRecordInterfacePtr & _record )
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