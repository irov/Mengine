#	include "LogEngine.h"
#	include "Application.h"

#	include <ctime>
#	include <cstdio>
#	include <stdarg.h>

#	include "python.h"
#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LogEngine::LogEngine( LogSystemInterface * _interface )
		: m_interface( _interface )
		, m_verboseLevel( LM_ERROR )
		, m_console( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::logMessage( const StringA& _message )
	{
		m_interface->logMessage( _message );
	}
	//////////////////////////////////////////////////////////////////////////
	LogSystemInterface * LogEngine::getInterface()
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::setVerboseLevel( int _level )
	{
		m_verboseLevel = _level;
	}
	//////////////////////////////////////////////////////////////////////////
	int LogEngine::getVerboseLevel() const
	{
		return m_verboseLevel;
	}
	//////////////////////////////////////////////////////////////////////////
	LoggerOperator::LoggerOperator( const char * _file, int _level, unsigned int _options )
		: m_file( _file )
		, m_level( _level )
		, m_options( _options )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerOperator::operator ()( const char* _message, ... )
	{
		if( Holder<LogEngine>::empty() )
		{
			return;
		}

		va_list argList;

		va_start(argList, _message);

		char str[1024];

		vsprintf( str, _message, argList );

		va_end(argList);

		const StringA& message( str );

		//bool isBreak = ( m_mask & ELoggerBreak ) > 0;

		//if( Py_IsInitialized() == 0 || !isBreak )
		//{
		//	bool isDebug = ( m_mask & ELoggerDebug ) > 0;

		//	Holder<LogEngine>::hostage()
		//		->logMessage( message, 1 );
		//}
		//else
		//{
		//	/*	PyErr_Format(PyExc_SystemError,
		//	"%s: %s"
		//	, const_cast<char*>(m_file)
		//	, message.c_str()				
		//	);*/
		//}
		Holder<LogEngine>::hostage()
			->logMessage( message );

		if( ( m_options & LO_MESSAGE_BOX ) != 0 )
		{
			Holder<LogEngine>::hostage()
				->logMessage( message );
			Holder<Application>::hostage()->showMessageBox( message, "Mengine Critical Error", 0 );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	Log::Log()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Log::~Log()
	{
		os << std::endl;
		Holder<LogEngine>::hostage()->logMessage( os.str() );
	}
	//////////////////////////////////////////////////////////////////////////
	std::ostringstream& Log::get( EMessageLevel level /* = LM_LOG */ )
	{
		os << Log::time() << ": ";
		return os;
	}
	//////////////////////////////////////////////////////////////////////////
	StringA Log::time()
	{
		std::time_t ctTime;
		std::time( &ctTime );
		std::tm* sTime = std::localtime( &ctTime );
		char result[9] = {0};
		sprintf_s( result, 9, "%02d:%02d:%02d", sTime->tm_hour, sTime->tm_min, sTime->tm_sec );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
}
