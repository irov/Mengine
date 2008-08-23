#	include "LogEngine.h"
#	include "Application.h"

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
		m_interface->startLog( MENGE_TEXT("Menge.log") );
		m_interface->logMessage( MENGE_TEXT("\n>>>> LOGGING STARTED\n\n >> Starting Mengine...\n\n"), false, true, true );
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::logMessage( const String& _message, bool _maskDebug, bool _endl, bool _timeStamp )
	{
		m_interface->logMessage( _message, _maskDebug, _endl, _timeStamp );
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
	void LoggerOperator::operator ()( const TChar * _message, ... )
	{
		if( Holder<LogEngine>::empty() )
		{
			return;
		}

		va_list argList;

		va_start(argList, _message);

		TChar str[1024];

#	ifdef MENGE_UNICODE
		vswprintf( str, 1024, _message, argList );
#	else
		vsprintf( str, _message, argList );
#	endif

		va_end(argList);

		const String& message( str );

		bool isBreak = ( m_mask & ELoggerBreak ) > 0;

		if( Py_IsInitialized() == 0 || !isBreak )
		{
			bool isDebug = ( m_mask & ELoggerDebug ) > 0;

			Holder<LogEngine>::hostage()
				->logMessage( message, isDebug );
		}
		else
		{
			/*	PyErr_Format(PyExc_SystemError,
			"%s: %s"
			, const_cast<char*>(m_file)
			, message.c_str()				
			);*/
		}

		if( ( m_mask & ELoggerMessageBox ) > 0 )
		{
			Holder<Application>::hostage()->showMessageBox( message, MENGE_TEXT("Mengine Critical Error"), 0 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
