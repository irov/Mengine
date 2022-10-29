#include "TimeoutGuardScope.h"

#include "Interface/TimeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/LoggerInterface.h"

#include "Kernel/String.h"
#include "Kernel/Logger.h"

#include "Config/StdArg.h"
#include "Config/StdIO.h"
#include "Config/StdIntTypes.h"

#include <chrono>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimeoutGuardScope::TimeoutGuardScope( TimeMilliseconds _timeout, const DocumentPtr & _doc, const Char * _format, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        MENGINE_VSNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, _format, args );

        MENGINE_VA_LIST_END( args );

        String message_str( message );

        TimeMilliseconds start = TIME_SYSTEM()
            ->getTimeMilliseconds();

        ThreadIdentityInterfacePtr threadIdentity = THREAD_SERVICE()
            ->createThreadIdentity( STRINGIZE_STRING_LOCAL( "TimeoutGuardScope" ), ETP_NORMAL, _doc );

        threadIdentity->run( [start, _timeout, message_str]( const ThreadIdentityRunnerInterfacePtr & _runner )
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

                THREAD_SYSTEM()
                    ->sleep( 100 );
            }

            if( _runner->isCancel() == true )
            {
                return;
            }

            LOGGER_ERROR( "TimeoutGuardScope [%" PRIu64 "] %s"
                , _timeout
                , message_str.c_str()
            );
        } );

        m_threadIdentity = threadIdentity;
    }
    //////////////////////////////////////////////////////////////////////////
    TimeoutGuardScope::~TimeoutGuardScope()
    {
        if( m_threadIdentity != nullptr )
        {
            THREAD_SERVICE()
                ->destroyThreadIdentity( m_threadIdentity );
            m_threadIdentity = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}