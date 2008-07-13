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
		m_interface->startLog( "Menge.log" );
		m_interface->logMessage( "\n>>>> LOGGING STARTED\n\n >> Starting Mengine...\n\n", false, true, true );
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::logMessage( const std::string & _message, bool _maskDebug, bool _endl, bool _timeStamp )
	{
		m_interface->logMessage( _message.c_str(), _maskDebug, _endl, _timeStamp );
	}
	//////////////////////////////////////////////////////////////////////////
	LogSystemInterface * LogEngine::getInterface()
	{
		return m_interface;
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
		
		va_start(argList, _message);

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