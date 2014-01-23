#	include "Logger.h"

#	include <ctime>
#	include <cstdio>
#	include <stdarg.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LoggerOperator::LoggerOperator( LogServiceInterface * _log, EMessageLevel _level, size_t _flag )
		: m_log(_log)
		, m_level(_level)
        , m_flag(_flag)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	LoggerOperator & LoggerOperator::operator ()( const char* _message, ... )
	{
		va_list argList;

		va_start(argList, _message);

		char str[2048];

		int size = vsnprintf( str, 2048 - 1, _message, argList );
        
		va_end(argList);

        if( size < 0 )
        {
            char msg [] = "LoggerOperator::operator invalid message :(\n";
            this->logMessage( msg, sizeof(msg) );

			size = snprintf( str, 2048 - 1, "%s", _message );

			if( size < 0 )
			{
				return *this;
			}

			str[size] = '\n';
			str[size + 1] = 0;	

			this->logMessage( _message, size + 1 );

            return *this;
        }
        
        str[size] = '\n';
        str[size + 1] = 0;	

		this->logMessage( str, size + 1 );

        return *this;
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerOperator::logMessage( const char * _message, size_t _size )
	{
		m_log->logMessage( m_level, m_flag, _message, _size );
	}
}
