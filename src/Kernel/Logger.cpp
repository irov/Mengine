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
    void LoggerOperator::logMessageTimeStamp() const
    {
        if( SERVICE_EXIST( PlatformInterface ) == true )
        {
            PlatformDateTime dateTime;
            PLATFORM_SERVICE()
                ->getDateTime( &dateTime );

            Char str_time[256] = { 0 };
            int size_time = sprintf( str_time, "%02u.%02u.%04u [%02u:%02u:%02u:%04u] "
                , dateTime.day
                , dateTime.month
                , dateTime.year
                , dateTime.hour
                , dateTime.minute
                , dateTime.second
                , dateTime.milliseconds
            );

            this->logMessage( str_time, size_time );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessageArgs( const Char * _format, va_list _args ) const
    {
        this->logMessageTimeStamp();

        Char str[MENGINE_LOGGER_MAX_MESSAGE] = { 0 };
        int32_t size = vsnprintf( str, MENGINE_LOGGER_MAX_MESSAGE, _format, _args );

        if( size < 0 )
        {
            const Char msg[] = "LoggerOperator::operator invalid message :(\n";
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

        if( size > (MENGINE_LOGGER_MAX_MESSAGE - 2) )
        {
            size = MENGINE_LOGGER_MAX_MESSAGE - 2;
        }

        str[size + 0] = '\n';
        str[size + 1] = 0;

        if( m_file != nullptr )
        {
            this->logMessageFunctionStamp( str, size );
        }
        else
        {
            this->logMessage( str, size + 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessageFunctionStamp( const Char * _msg, uint32_t _size ) const
    {
        Char str2[MENGINE_LOGGER_MAX_MESSAGE + 256] = {0};

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

        int32_t size2 = snprintf( str2, MENGINE_LOGGER_MAX_MESSAGE + 256, "%s[%u]: %.*s"
            , str_function.c_str()
            , m_line
            , _size
            , _msg
        );

		if( size2 == -1 )
		{
			return;
		}

        str2[size2 + 0] = '\n';
        str2[size2 + 1] = 0;

        this->logMessage( str2, size2 + 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessage( const Char * _msg, uint32_t _size ) const
    {
        LOGGER_SERVICE()
            ->logMessage( m_level, m_flag, _msg, _size );
    }
}
