#include "Crash.h"

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#include "Kernel/NotificationHelper.h"
#include "Kernel/DebugBreak.h"
#include "Kernel/ConfigurationHelper.h"

#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        void crash( const Char * _doc )
        {
            if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
            {
                NOTIFICATION_NOTIFY( NOTIFICATOR_CRASH, _doc );
            }

#if defined(MENGINE_DEBUG)
            Helper::debuggerBreak();
#endif

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
            // Same rationale as Helper::abort: self-contained popup gated
            // by the engine-wide silent-dialog flag on ServiceProvider.
            if( Helper::isSilentDialog() == false )
            {
                ::MessageBoxA( NULL, _doc, "Mengine crash", MB_OK | MB_ICONERROR );
            }
#endif

            volatile uint32_t * p = nullptr;
            
            // cppcheck-suppress nullPointer
            *p = 0x7BADC0DE;
        }
    }
}
