#include "Logger.h"

#include <ctime>
#include <cstdio>
#include <stdarg.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_LOGGER_MAX_MESSAGE 8192
	//////////////////////////////////////////////////////////////////////////
	LoggerOperator::LoggerOperator( LoggerServiceInterface * _log, EMessageLevel _level, uint32_t _flag )
		: m_log(_log)
		, m_level(_level)
        , m_flag(_flag)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	LoggerOperator & LoggerOperator::operator () ( const char * _format, ... )
	{
		va_list argList;

		va_start(argList, _format);

        char str[MENGINE_LOGGER_MAX_MESSAGE] = { 0 };
		int size = vsnprintf( str, MENGINE_LOGGER_MAX_MESSAGE, _format, argList );
        
		va_end(argList);

        if( size < 0 )
        {
            const char msg [] = "LoggerOperator::operator invalid message :(\n";
            this->logMessage( msg, sizeof(msg) );

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

		this->logMessage( str, size + 1 );

        return *this;
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerOperator::logMessage( const Char * _format, uint32_t _size )
	{
		m_log->logMessage( m_level, m_flag, _format, _size );
	}
}
