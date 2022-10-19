#include "TimeoutGuardScope.h"

#include "Interface/TimeSystemInterface.h"
#include "Interface/LoggerInterface.h"

#include "Kernel/String.h"
#include "Kernel/Logger.h"

#include "Config/StdIO.h"
#include "Config/StdIntTypes.h"

#include <chrono>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimeoutGuardScope::TimeoutGuardScope( TimeMilliseconds _timeout, const Char * _format, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        MENGINE_VSNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, _format, args );

        MENGINE_VA_LIST_END( args );

        String message_str( message );

        AtomicBool * progress = new AtomicBool( true );

        TimeMilliseconds start = TIME_SYSTEM()
            ->getTimeMilliseconds();

        m_thread = std::thread( [progress, start, _timeout]( const String & _message )
        {
            for( ;; )
            {
                TimeMilliseconds current = TIME_SYSTEM()
                    ->getTimeMilliseconds();

                TimeMilliseconds offset = current - start;

                if( offset > _timeout )
                {
                    break;
                }

                std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
            }

            if( *progress == false )
            {
                return;
            }

            LOGGER_ERROR( "TimeoutGuardScope [%" PRIu64 "] %s"
                , _timeout
                , _message.c_str()
            );
        }, message_str );

        m_progress = progress;
    }
    //////////////////////////////////////////////////////////////////////////
    TimeoutGuardScope::~TimeoutGuardScope()
    {
        *m_progress = false;
        m_thread.detach();
    }
    //////////////////////////////////////////////////////////////////////////
}