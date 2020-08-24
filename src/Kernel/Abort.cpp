#include "Crash.h"

#include "Interface/NotificationServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        void abort()
        {
            if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
            {
                NOTIFICATION_NOTIFY( NOTIFICATOR_ABORT );
            }

            ::abort();
        }
    }
}