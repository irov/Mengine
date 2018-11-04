#include "Logger.h"

#include "Config/String.h"
#include "Config/StringRegex.h"

#include <ctime>
#include <cstdio>
#include <stdarg.h>

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
    LoggerOperator & LoggerOperator::operator () ( const char * _format, ... )
    {
        va_list argList;

        va_start( argList, _format );

        Char str[MENGINE_LOGGER_MAX_MESSAGE] = { 0 };
        int size = vsnprintf( str, MENGINE_LOGGER_MAX_MESSAGE, _format, argList );

        va_end( argList );

        if( size < 0 )
        {
            const Char msg[] = "LoggerOperator::operator invalid message :(\n";
            this->logMessage( msg, sizeof( msg ) );

            size = sprintf( str, "%s", _format );

            if( size < 0 )
            {
                return *this;
            }

            str[size] = '\n';
            str[size + 1] = 0;

            this->logMessage( _format, size + 1 );

            return *this;
        }

        if( size > (MENGINE_LOGGER_MAX_MESSAGE - 2) )
        {
            size = MENGINE_LOGGER_MAX_MESSAGE - 2;
        }

        str[size + 0] = '\n';
        str[size + 1] = 0;

        if( m_file != nullptr )
        {
            Char str2[MENGINE_LOGGER_MAX_MESSAGE + 256] = { 0 };

            //const Char * correct_function = m_function;
            //if( strstr( correct_function, "Mengine::" ) != nullptr )
            //{
            //    correct_function += sizeof( "Mengine::" ) - 1;
            //}

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

            int size2 = snprintf( str2, MENGINE_LOGGER_MAX_MESSAGE + 256, "%s[%u]: %.*s\n"
                , str_function.c_str()
                , m_line
                , size
                , str
            );

            this->logMessage( str2, size2 + 1 );
        }
        else
        {
            this->logMessage( str, size + 1 );
        }

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerOperator::logMessage( const Char * _format, uint32_t _size )
    {
        LOGGER_SERVICE()
            ->logMessage( m_level, m_flag, _format, _size );
    }
}
