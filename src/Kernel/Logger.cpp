#include "Logger.h"

#include "Interface/LoggerServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        bool loggerValidMessage( const ConstString & _category, ELoggerLevel _level, uint32_t _filter )
        {
            bool result = LOGGER_SERVICE()
                ->validMessage( _category, _level, _filter );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator::LoggerOperator( const ConstString & _category, ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _file, uint32_t _line, uint32_t _flag )
        : m_category( _category )
        , m_level( _level )
        , m_filter( _filter )
        , m_color( _color )
        , m_file( _file )
        , m_line( _line )
        , m_flag( _flag )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator::~LoggerOperator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const LoggerOperator & LoggerOperator::operator()( const Char * _format, ... ) const
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        this->logMessageArgs( _format, args );

        MENGINE_VA_LIST_END( args );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessageArgs( const Char * _format, MENGINE_VA_LIST_TYPE _args ) const
    {
        Char str[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, MENGINE_LOGGER_MAX_MESSAGE - 2, _format, _args );

        if( size_vsnprintf >= 0 )
        {
            this->logMessage( m_color, str, size_vsnprintf );
        }
        else
        {
            int32_t size_snprintf = MENGINE_SNPRINTF( str, MENGINE_LOGGER_MAX_MESSAGE - 2, "invalid message format: %s"
                , _format
            );

            if( size_snprintf < 0 )
            {
                return;
            }

            this->logMessage( LCOLOR_RED, str, size_vsnprintf );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessage( uint32_t _color, const Char * _data, size_t _size ) const
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        LoggerMessage msg;
        msg.category = m_category;
        msg.dateTime = dateTime;
        msg.level = m_level;
        msg.flag = m_flag;
        msg.filter = m_filter;
        msg.color = _color;
        msg.file = m_file;
        msg.line = m_line;
        msg.data = _data;
        msg.size = _size;

        LOGGER_SERVICE()
            ->logMessage( msg );
    }
    //////////////////////////////////////////////////////////////////////////
}
