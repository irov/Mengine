#	include "LogEngine.h"

#	include <stdio.h>
#	include <sstream>
#	include <stdarg.h>

#	include "python.h"

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
	LoggerOperator::LoggerOperator( const char * _file, unsigned int _mask )
		: m_file(_file)
		, m_mask(_mask)
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

		bool isBreak = ( m_mask & ELoggerBreak ) > 0;

		if( Py_IsInitialized() == 0 || !isBreak )
		{
			bool isDebug = ( m_mask & ELoggerDebug ) > 0;

			Holder<LogEngine>::hostage()
				->logMessage( message, isDebug );
		}
		else
		{
			PyErr_Format(PyExc_SystemError,
				"%s: %s"
				, const_cast<char*>(m_file)
				, message.c_str()				
				);
		}
	}
}
