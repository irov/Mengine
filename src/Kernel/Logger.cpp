#include "Logger.h"

#include "Interface/LoggerServiceInterface.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        bool loggerValidMessage( ELoggerLevel _level, uint32_t _flag )
        {
            bool result = LOGGER_SERVICE()
                ->validMessage( _level, _flag );

            return result;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator::LoggerOperator( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _file, uint32_t _line )
        : m_level( _level )
        , m_flag( _flag )
        , m_color( _color )
        , m_file( _file )
        , m_line( _line )
        , m_newline( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator::~LoggerOperator()
    {
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
        Char str[MENGINE_LOGGER_MAX_MESSAGE] = { 0 };

        size_t size = 0;

        size_t size_timestamp = LOGGER_SERVICE()
            ->makeTimeStamp( str, size, MENGINE_LOGGER_MAX_MESSAGE );

        if( size_timestamp > 0 )
        {
            size += size_timestamp;
        }

        size_t size_functionstamp = LOGGER_SERVICE()
            ->makeFunctionStamp( m_file, m_line, str, size, MENGINE_LOGGER_MAX_MESSAGE );

        if( size_functionstamp > 0 )
        {
            size += size_functionstamp;
        }

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str + size, MENGINE_LOGGER_MAX_MESSAGE - size - 2, _format, _args );

        if( size_vsnprintf > 0 )
        {
            size += size_vsnprintf;
        }
        
        if( size < 0 )
        {
            const Char msg[] = "invalid message :(\n";
            this->logMessage( msg, sizeof( msg ) );

            int32_t size_sprintf = MENGINE_SPRINTF( str, "%s", _format );

            if( size_sprintf < 0 )
            {
                return;
            }

            size += size_sprintf;

            str[size] = '\n';
            str[size + 1] = '\0';

            this->logMessage( _format, (size_t)(size + 1) );

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

            this->logMessage( str, (size_t)(size + 1) );
        }
        else
        {
            if( size > MENGINE_LOGGER_MAX_MESSAGE - 1 )
            {
                size = MENGINE_LOGGER_MAX_MESSAGE - 1;
            }

            str[size + 0] = '\0';

            this->logMessage( str, size );
        }        
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessage( const Char * _msg, size_t _size ) const
    {
        LOGGER_SERVICE()
            ->logMessage( m_level, m_flag, m_color, _msg, _size );
    }
}
