#include "Logger.h"

#include "Interface/PlatformInterface.h"

#include "Config/String.h"
#include "Config/StringRegex.h"

#include <ctime>
#include <cstdio>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_LOGGER_MAX_MESSAGE
#define MENGINE_LOGGER_MAX_MESSAGE 8192
#endif
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator::LoggerOperator( EMessageLevel _level, uint32_t _flag, const Char * _file, uint32_t _line )
        : m_level( _level )
        , m_flag( _flag )
        , m_file( _file )
        , m_line( _line )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerOperator::~LoggerOperator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const LoggerOperator & LoggerOperator::operator () ( const Char * _format, ... ) const
    {
        va_list args;
        va_start( args, _format );

        this->logMessageArgs( _format, args );

        va_end( args );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t LoggerOperator::makeTimeStamp( Char * _buffer, int32_t _offset ) const
    {
        if( SERVICE_EXIST( PlatformInterface ) == false )
        {
            return 0;
        }

        PlatformDateTime dateTime;
        PLATFORM_SERVICE()
            ->getDateTime( &dateTime );

        int size = snprintf( _buffer + _offset, MENGINE_LOGGER_MAX_MESSAGE - _offset, "[%02u:%02u:%02u:%04u] "
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
            , dateTime.milliseconds
        );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t LoggerOperator::makeFunctionStamp( Char * _buffer, int32_t _offset ) const
    {
        if( m_file == nullptr )
        {
            return 0;
        }

        String str_function = m_file;

        StringRegex regex_lambda_remove( "::<lambda_.*>::operator \\(\\)" );

        StringMatchResults match_lambda_remove;
        while( std::regex_search( str_function, match_lambda_remove, regex_lambda_remove ) == true )
        {
            const std::sub_match<String::const_iterator> & lambda_remove_prefix = match_lambda_remove.prefix();
            const std::sub_match<String::const_iterator> & lambda_remove_suffix = match_lambda_remove.suffix();

            str_function = String( lambda_remove_prefix.first, lambda_remove_prefix.second ) + String( lambda_remove_suffix.first, lambda_remove_suffix.second );
        }

        StringRegex regex_engine_remove( "Mengine::" );

        StringMatchResults match_engine_remove;
        if( std::regex_search( str_function, match_engine_remove, regex_engine_remove ) == true )
        {
            const std::sub_match<String::const_iterator> & engine_remove_prefix = match_engine_remove.prefix();
            const std::sub_match<String::const_iterator> & engine_remove_suffix = match_engine_remove.suffix();

            str_function = String( engine_remove_prefix.first, engine_remove_prefix.second ) + String( engine_remove_suffix.first, engine_remove_suffix.second );
        }

        int32_t size = snprintf( _buffer + _offset, MENGINE_LOGGER_MAX_MESSAGE - _offset, "%s[%u] "
            , str_function.c_str()
            , m_line
        );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessageArgs( const Char * _format, va_list _args ) const
    {
        Char str[MENGINE_LOGGER_MAX_MESSAGE] = { 0 };

        int size = 0;

        size += this->makeTimeStamp( str, size );
        size += this->makeFunctionStamp( str, size );

        size += vsnprintf( str + size, MENGINE_LOGGER_MAX_MESSAGE - size - 2, _format, _args );

        if( size < 0 )
        {
            const Char msg[] = "invalid message :(\n";
            this->logMessage( msg, sizeof( msg ) );

            size = sprintf( str, "%s", _format );

            if( size < 0 )
            {
                return;
            }

            str[size] = '\n';
            str[size + 1] = 0;

            this->logMessage( _format, size + 1 );

            return;
        }

        str[size + 0] = '\n';
        str[size + 1] = 0;

        this->logMessage( str, size + 1 );
    }    
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessage( const Char * _msg, uint32_t _size ) const
    {
        LOGGER_SERVICE()
            ->logMessage( m_level, m_flag, _msg, _size );
    }
}
