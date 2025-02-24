#include "Abort.h"

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#include "Kernel/NotificationHelper.h"
#include "Kernel/DebugBreak.h"

#include "Config/StdLib.h"

namespace Mengine
{
    namespace Helper
    {
        void abort( const Char * _doc )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ABORT, _doc );

#if defined(MENGINE_DEBUG)
            Helper::debuggerBreak();
#endif
            
#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
            ::MessageBoxA( NULL, _doc, "Mengine abort", MB_OK );
#endif

            ::abort();
        }
    }
}
