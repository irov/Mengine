#include "Error.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Abort.h"
#include "Kernel/DebugBreak.h"
#include "Kernel/PlatformLogFormat.h"

#include "Kernel/NotificationHelper.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ErrorOperator::ErrorOperator( const Char * _category, EErrorLevel _level, const Char * _file, uint32_t _line, const Char * _function )
            : m_category( _category )
            , m_level( _level )
            , m_file( _file )
            , m_line( _line )
            , m_function( _function )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        ErrorOperator::~ErrorOperator()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        void ErrorOperator::operator () ( const Char * _format, ... ) const
        {
            Char message_info[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );
            MENGINE_VSNPRINTF( message_info, MENGINE_LOGGER_MAX_MESSAGE, _format, args );
            MENGINE_VA_LIST_END( args );

            Char error_info[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};
            MENGINE_SNPRINTF( error_info, MENGINE_LOGGER_MAX_MESSAGE, "%s[%d] Error [%s]: %s"
                , m_file
                , m_line
                , m_category
                , message_info
            );

            if( SERVICE_IS_INITIALIZE( LoggerServiceInterface ) == true )
            {
                LOGGER_VERBOSE_LEVEL( m_category, LM_ERROR, LFILTER_NONE, LCOLOR_RED, m_file, m_line, m_function, LFLAG_NONE )("%s"
                    , error_info
                    );
            }
            else
            {
                Helper::PlatformLogMessage( error_info );
            }

            NOTIFICATION_NOTIFY( NOTIFICATOR_ERROR, m_category, m_level, m_file, m_line, m_function, message_info );

#if defined(MENGINE_DEBUG)
            Helper::debuggerBreak();
#endif
            
            switch( m_level )
            {
            case ERROR_LEVEL_FATAL:
                {
                    Helper::abort( message_info );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
