#	include "LogEngine.h"

#	include <stdio.h>
#	include <sstream>
#	include <stdarg.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LogEngine::LogEngine( LogSystemInterface * _interface )
		: m_interface( _interface )
	{
		Holder<LogEngine>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::logMessage( const std::string & _message, bool _maskDebug )
	{
		m_interface->logMessage( _message.c_str(), _maskDebug );
	}
	//////////////////////////////////////////////////////////////////////////
	LoggerOperator::LoggerOperator( const char * _file, bool _maskDebug )
		: m_file(_file)
		, m_maskDebug(_maskDebug)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerOperator::operator ()( const char * _message, ... )
	{
		if( Holder<LogEngine>::empty() )
		{
			return;
		}

		va_list argList;
		//Initialize variable argument list
		va_start(argList, _message);

		//Write the error to the log file

		char str[1024];

		vsprintf_s( str, _message, argList );

		va_end(argList);
		std::stringstream strMessage;
		strMessage <</* m_file << std::endl <<*/ str;

		const std::string & message = strMessage.str();

		Holder<LogEngine>::hostage()
			->logMessage( message, m_maskDebug );
	}
}