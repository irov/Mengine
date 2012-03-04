#	include "Logger.h"

#	include <ctime>
#	include <cstdio>
#	include <stdarg.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LoggerOperator::LoggerOperator( LogServiceInterface * _log, EMessageLevel _level )
		: m_log(_log)
		, m_level(_level)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerOperator::operator ()( const char* _message, ... )
	{
		va_list argList;

		va_start(argList, _message);

		char str[1024];

		vsprintf( str, _message, argList );

		va_end(argList);

		String message( str );
		message += '\n';

		this->logMessage( message );
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerOperator::logMessage( const std::string & _message )
	{
		m_log->logMessage( m_level, _message );
	}
}
