#	include "Logger.h"

#	include <ctime>
#	include <cstdio>
#	include <stdarg.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LoggerOperator::LoggerOperator( LogSystemInterface * _log, const char * _file, EMessageLevel _level )
		: m_log(_log)
		, m_file(_file)
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

		m_log->logMessage( message, m_level );
	}
}
