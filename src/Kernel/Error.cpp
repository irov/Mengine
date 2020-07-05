#include "Error.h"
#include "Logger.h"

#include "Interface/PlatformInterface.h"
#include "Interface/NotificationServiceInterface.h"

#ifdef MENGINE_PLATFORM_WINDOWS
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#include "Config/StdIO.h"

#ifndef MENGINE_ERROR_MAX_MESSAGE
#define MENGINE_ERROR_MAX_MESSAGE 8192
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ErrorOperator::ErrorOperator( uint32_t _level, const Char * _file, uint32_t _line )
        : m_level( _level )
        , m_file( _file )
        , m_line( _line )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ErrorOperator::~ErrorOperator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ErrorOperator::operator () ( const Char * _format, ... ) const
    {
        Char str_info[MENGINE_ERROR_MAX_MESSAGE] = {0};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str_info, MENGINE_ERROR_MAX_MESSAGE - 1, _format, args );

        MENGINE_VA_LIST_END( args );

        if( size_vsnprintf >= 0 )
        {
            LoggerOperator( LM_ERROR, 0, LCOLOR_RED, m_file, m_line )
                .logMessage( str_info, (size_t)size_vsnprintf );
        }

        switch( m_level )
        {
        case ERROR_LEVEL_MESSAGE:
            {
                if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_ERROR, m_level, m_file, m_line, str_info );
                }
            }break;
        case ERROR_LEVEL_FATAL:
            {
                PLATFORM_SERVICE()
                    ->abort();
            }break;
        }

#ifdef MENGINE_PLATFORM_WINDOWS
        if( ::IsDebuggerPresent() == TRUE )
        {
            ::DebugBreak();
        }
#endif
    }
}
