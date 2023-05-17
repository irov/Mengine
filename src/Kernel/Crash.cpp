#include "Crash.h"

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#include "Kernel/NotificationHelper.h"
#include "Kernel/DebugBreak.h"

#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        void crash( const Char * _doc )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_CRASH, _doc );

            Helper::debuggerBreak();

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
            ::MessageBoxA( NULL, _doc, "Mengine crash", MB_OK );
#endif
            
            volatile uint32_t * p = nullptr;

            // cppcheck-suppress nullPointer
            *p = 0x7BADC0DE;
        }
    }
}