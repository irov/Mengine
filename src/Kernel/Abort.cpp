#include "Abort.h"

#include "Kernel/NotificationHelper.h"

#include "Config/StdLib.h"

namespace Mengine
{
    namespace Helper
    {
        void abort( const Char * _doc )
        {
            if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
            {
                NOTIFICATION_NOTIFY( NOTIFICATOR_ABORT, _doc );
            }

            ::abort();
        }
    }
}
