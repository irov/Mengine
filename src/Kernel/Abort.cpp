#include "Abort.h"

#ifdef MENGINE_PLATFORM_WINDOWS
#include "Environment/Windows/WindowsIncluder.h"
#endif

#include "Kernel/NotificationHelper.h"

#include "Config/StdLib.h"

namespace Mengine
{
    namespace Helper
    {
        void abort( const Char * _doc )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ABORT, _doc );
            
#ifdef MENGINE_PLATFORM_WINDOWS
            ::MessageBoxA( NULL, _doc, "Mengine abort", MB_OK );
#endif

            ::abort();
        }
    }
}
