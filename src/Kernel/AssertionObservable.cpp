#include "AssertionObservable.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Interface/NotificationServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        bool assertionHasObserver( Observable * _observer )
        {
            if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == false )
            {
                return false;
            }

            bool observer = NOTIFICATION_SERVICE()
                ->hasObserver( _observer );

            return observer;
        }
    }
}
#endif