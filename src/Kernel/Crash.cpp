#include "Crash.h"

#include "Interface/NotificationServiceInterface.h"

#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        void crash()
        {
            if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
            {
                NOTIFICATION_NOTIFY( NOTIFICATOR_CRASH );
            }

            volatile uint32_t * p = nullptr;

            // cppcheck-suppress nullPointer
            *p = 0x7BADC0DE;
        }
    }
}