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
        bool loggerValidMessage( const ConstString & _category, ELoggerLevel _level, uint32_t _filter )
        {
            bool result = LOGGER_SERVICE()
                ->validMessage( _category, _level, _filter );

            return result;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator::LoggerOperator( const ConstString & _category, ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _file, uint32_t _line, ELoggerFlag _flag )
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
    const LoggerOperator & LoggerOperator::operator () ( const Char * _format, ... ) const
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
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        Char str[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        size_t size = 0;

        if( m_flag & ELF_FLAG_FUNCTION_STAMP )
        {
            size_t size_functionstamp = Helper::makeLoggerFunctionStamp( m_file, m_line, str, size, MENGINE_LOGGER_MAX_MESSAGE - size - 2 );

            if( size_functionstamp > 0 )
            {
                size += size_functionstamp;
            }
        }

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str + size, MENGINE_LOGGER_MAX_MESSAGE - size - 2, _format, _args );

        if( size_vsnprintf >= 0 )
        {
            size += size_vsnprintf;
        }
        else
        {
            int32_t size_snprintf = MENGINE_SNPRINTF( str + size, MENGINE_LOGGER_MAX_MESSAGE - size - 2, "invalid message format: %s"
                , _format
            );

            if( size_snprintf < 0 )
            {
                return;
            }

            size += size_snprintf;

            this->logMessage( dateTime, LCOLOR_RED, str, size );

            return;
        }

        this->logMessage( dateTime, m_color, str, size );        
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessage( const PlatformDateTime & _dateTime, uint32_t _color, const Char * _data, size_t _size ) const
    {
        LoggerMessage msg;
        msg.category = m_category;
        msg.dateTime = _dateTime;
        msg.level = m_level;
        msg.filter = m_filter;
        msg.color = _color;
        msg.data = _data;
        msg.size = _size;

        LOGGER_SERVICE()
            ->logMessage( msg );
    }
    //////////////////////////////////////////////////////////////////////////
}
