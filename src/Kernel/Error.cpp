#include "Error.h"

#include "Interface/LoggerInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Abort.h"
#include "Kernel/DebugBreak.h"

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#include "Kernel/NotificationHelper.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ErrorOperator::ErrorOperator( EErrorLevel _level, const Char * _file, uint32_t _line )
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
        Char str_info[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str_info, MENGINE_LOGGER_MAX_MESSAGE - 2, _format, args );

        MENGINE_VA_LIST_END( args );

        if( size_vsnprintf >= 0 )
        {
            str_info[size_vsnprintf + 0] = '\n';
            str_info[size_vsnprintf + 1] = '\0';

            LoggerOperator( ConstString::none(), LM_ERROR, 0, LCOLOR_RED, m_file, m_line, LFLAG_FULL )("%s", str_info);
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_ERROR, m_level, m_file, m_line, str_info );
        
        switch( m_level )
        {
        case ERROR_LEVEL_MESSAGE:
            {
                //Empty
            }break;
        case ERROR_LEVEL_FATAL:
            {
                Helper::abort( str_info );
            }break;
        }

        Helper::debuggerBreak();
    }
    //////////////////////////////////////////////////////////////////////////
}
