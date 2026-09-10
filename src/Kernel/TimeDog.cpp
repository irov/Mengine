#include "TimeDog.h"

#include "Kernel/TimestampHelper.h"

#include "Config/StdArg.h"
#include "Config/StdIO.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimeDogScope::TimeDogScope( const Char * _category, const Char * _file, int32_t _line, const Char * _function, const Char * _format, ... )
        : m_file( _file )
        , m_function( _function )
        , m_line( _line )
        , m_beginTime( 0.0 )
        , m_enabled( false )
    {
        if( Detail::loggerValidMessage( _category, LM_INFO, LFILTER_NONE ) == false )
        {
            return;
        }

        StdString::strcpy_safe( m_category, _category, MENGINE_LOGGER_MAX_CATEGORY );

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t messageSize = MENGINE_VSNPRINTF( m_message, MENGINE_LOGGER_MAX_MESSAGE, _format, args );

        MENGINE_VA_LIST_END( args );

        if( messageSize < 0 )
        {
            return;
        }

        m_beginTime = Helper::getElapsedTime();
        m_enabled = true;
    }
    //////////////////////////////////////////////////////////////////////////
    TimeDogScope::~TimeDogScope()
    {
        if( m_enabled == false )
        {
            return;
        }

        if( Detail::loggerValidMessage( m_category, LM_INFO, LFILTER_NONE ) == false )
        {
            return;
        }

        double elapsed = Helper::getElapsedTime() - m_beginTime;

        LoggerOperator( m_category, LM_INFO, LFILTER_NONE, LCOLOR_GREEN | LCOLOR_BLUE, m_file, m_line, m_function, LFLAG_SHORT )( "%s %.3f ms"
            , m_message
            , elapsed
        );
    }
    //////////////////////////////////////////////////////////////////////////
}
