#include "Crash.h"

#include "Kernel/NotificationHelper.h"

#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        void crash( const Char * _doc )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_CRASH, _doc );
            
            volatile uint32_t * p = nullptr;

            // cppcheck-suppress nullPointer
            *p = 0x7BADC0DE;
        }
    }
}