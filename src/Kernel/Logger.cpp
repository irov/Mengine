#include "Logger.h"

#include "Interface/LoggerServiceInterface.h"

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
    LoggerOperator::LoggerOperator( const ConstString & _category, ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _file, uint32_t _line )
        : m_category( _category )
        , m_level( _level )
        , m_filter( _filter )
        , m_color( _color )
        , m_file( _file )
        , m_line( _line )
        , m_timestamp( true )
        , m_newline( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator::~LoggerOperator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator & LoggerOperator::setTimestamp( bool _timestamp )
    {
        m_timestamp = _timestamp;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerOperator::getTimestamp() const
    {
        return m_timestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator & LoggerOperator::setNewline( bool _newline )
    {
        m_newline = _newline;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerOperator::getNewline() const
    {
        return m_newline;
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
        Char str[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        size_t size = 0;

        if( m_timestamp == true )
        {
            if( m_level > LM_CRITICAL )
            {
                size_t size_timestamp = LOGGER_SERVICE()
                    ->makeTimeStamp( str + size, size, MENGINE_LOGGER_MAX_MESSAGE - size - 2 );

                if( size_timestamp > 0 )
                {
                    size += size_timestamp;
                }

                if( m_category.empty() == false )
                {
                    size_t size_category = MENGINE_SNPRINTF( str + size, MENGINE_LOGGER_MAX_MESSAGE - size - 2, "[%s] "
                        , m_category.c_str()
                    );

                    if( size_category > 0 )
                    {
                        size += size_category;
                    }
                }

                size_t size_functionstamp = LOGGER_SERVICE()
                    ->makeFunctionStamp( m_file, m_line, str, size, MENGINE_LOGGER_MAX_MESSAGE - size - 2 );

                if( size_functionstamp > 0 )
                {
                    size += size_functionstamp;
                }
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

            str[size + 0] = '\n';
            str[size + 1] = '\0';

            this->logMessage( LCOLOR_RED, str, size + 1 );

            return;
        }

        if( m_newline == true )
        {
            if( size > MENGINE_LOGGER_MAX_MESSAGE - 2 )
            {
                size = MENGINE_LOGGER_MAX_MESSAGE - 2;
            }

            str[size + 0] = '\n';
            str[size + 1] = '\0';

            this->logMessage( m_color, str, size + 1 );
        }
        else
        {
            if( size > MENGINE_LOGGER_MAX_MESSAGE - 1 )
            {
                size = MENGINE_LOGGER_MAX_MESSAGE - 1;
            }

            str[size + 0] = '\0';

            this->logMessage( m_color, str, size );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessage( uint32_t _color, const Char * _msg, size_t _size ) const
    {
        LOGGER_SERVICE()
            ->logMessage( m_level, m_filter, _color, _msg, _size );
    }
    //////////////////////////////////////////////////////////////////////////
}
