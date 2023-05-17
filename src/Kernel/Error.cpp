#include "Error.h"

#include "Interface/LoggerInterface.h"

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
        ErrorOperator::ErrorOperator( const Char * _category, EErrorLevel _level, const Char * _file, uint32_t _line )
            : m_category( _category )
            , m_level( _level )
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
            Char error_info[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            int32_t size_vsnprintf = MENGINE_VSNPRINTF( error_info, MENGINE_LOGGER_MAX_MESSAGE - 2, _format, args );

            MENGINE_VA_LIST_END( args );

            if( size_vsnprintf >= 0 )
            {
                error_info[size_vsnprintf + 0] = '\n';
                error_info[size_vsnprintf + 1] = '\0';

                LoggerOperator( m_category, LM_ERROR, 0, LCOLOR_RED, m_file, m_line, LFLAG_FULL )("%s", error_info);
            }

            Helper::PlatformLogFormat( "|Error| %s:%u [%s] %s"
                , m_file
                , m_line
                , m_category
                , error_info
            );

            NOTIFICATION_NOTIFY( NOTIFICATOR_ERROR, m_category, m_level, m_file, m_line, error_info );

            switch( m_level )
            {
            case ERROR_LEVEL_MESSAGE:
                {
                    //Empty
                }break;
            case ERROR_LEVEL_FATAL:
                {
                    Helper::abort( error_info );
                }break;
            }

            Helper::debuggerBreak();
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
