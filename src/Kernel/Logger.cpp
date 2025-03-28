#include "Logger.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        bool loggerValidMessage( const Char * _category, ELoggerLevel _level, uint32_t _filter )
        {
            if( SERVICE_IS_INITIALIZE( LoggerServiceInterface ) == false )
            {
                return false;
            }

            bool result = LOGGER_SERVICE()
                ->validMessage( _category, _level, _filter );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator::LoggerOperator( const Char * _category, ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _file, int32_t _line, const Char * _function, uint32_t _flag )
        : m_category( _category )
        , m_level( _level )
        , m_filter( _filter )
        , m_color( _color )
        , m_file( _file )
        , m_function( _function )
        , m_line( _line )
        , m_flag( _flag )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator::~LoggerOperator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::operator()( const Char * _format, ... ) const
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        this->logMessageArgs( _format, args );

        MENGINE_VA_LIST_END( args );
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessageArgs( const Char * _format, MENGINE_VA_LIST_TYPE _args ) const
    {
        Char str[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};
        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, MENGINE_LOGGER_MAX_MESSAGE - 2, _format, _args );

        if( size_vsnprintf < 0 )
        {
            int32_t size_snprintf = MENGINE_SNPRINTF( str, MENGINE_LOGGER_MAX_MESSAGE - 2, "invalid message format: %s"
                , _format
            );

            if( size_snprintf < 0 )
            {
                return;
            }

            this->logMessage( LCOLOR_RED, str, size_snprintf );

            return;
        }

        if( size_vsnprintf == 0 )
        {
            return;
        }

        this->logMessage( m_color, str, size_vsnprintf );
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessage( uint32_t _color, const Char * _data, size_t _size ) const
    {
        LoggerMessage msg;
        msg.timestamp = Helper::getLocalTimestamp();
        msg.category = m_category;
        msg.threadName = Helper::getCurrentThreadName();
        msg.level = m_level;
        msg.flag = m_flag;
        msg.filter = m_filter;
        msg.color = _color;
        msg.file = m_file;
        msg.function = m_function;
        msg.line = m_line;
        msg.data = _data;
        msg.size = _size;

        LOGGER_SERVICE()
            ->logMessage( msg );
    }
    //////////////////////////////////////////////////////////////////////////
}
