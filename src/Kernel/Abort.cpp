#include "Abort.h"

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#include "Kernel/NotificationHelper.h"
#include "Kernel/DebugBreak.h"
#include "Kernel/ConfigurationHelper.h"

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
            // Self-contained Win32 popup gated by the engine-wide
            // silent-dialog flag carried on ServiceProvider. No dependency
            // on PlatformService: this runs even during late teardown when
            // services may already be gone.
            if( Helper::isSilentDialog() == false )
            {
                ::MessageBoxA( NULL, _doc, "Mengine abort", MB_OK | MB_ICONERROR );
            }
#endif

            ::abort();
        }
    }
}
