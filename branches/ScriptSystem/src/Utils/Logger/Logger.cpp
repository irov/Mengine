#	include "Logger.h"

#	include <ctime>
#	include <cstdio>
#	include <stdarg.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Logger::Logger()
		: m_interface( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Logger::~Logger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Logger::initialize( LogSystemInterface * _interface )
	{
		m_interface = _interface;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Logger::logMessage( const String& _message, EMessageLevel _log /*= LM_LOG */ )
	{
		m_interface->logMessage( _message, _log );
	}
	//////////////////////////////////////////////////////////////////////////
	LogSystemInterface * Logger::getInterface()
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	void Logger::setVerboseLevel( EMessageLevel _level )
	{
		m_interface->setVerboseLevel( _level );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Logger::registerLogger( OutputStreamInterface* _logger )
	{
		return m_interface->registerLogger( _logger );
	}
	//////////////////////////////////////////////////////////////////////////
	void Logger::unregisterLogger( OutputStreamInterface* _logger )
	{
		m_interface->unregisterLogger( _logger );
	}
	//////////////////////////////////////////////////////////////////////////
	LoggerOperator::LoggerOperator( const char * _file, EMessageLevel _level )
		: m_file( _file )
		, m_level( _level )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerOperator::operator ()( const char* _message, ... )
	{
		if( Holder<Logger>::empty() )
		{
			return;
		}

		va_list argList;

		va_start(argList, _message);

		char str[1024];

		vsprintf( str, _message, argList );

		va_end(argList);

		String message( str );
		message += '\n';

		//bool isBreak = ( m_mask & ELoggerBreak ) > 0;

		//if( Py_IsInitialized() == 0 || !isBreak )
		//{
		//	bool isDebug = ( m_mask & ELoggerDebug ) > 0;

		//	Holder<Logger>::hostage()
		//		->logMessage( message, 1 );
		//}
		//else
		//{
		//		PyErr_Format(PyExc_SystemError,
		//	"%s: %s"
		//	, const_cast<char*>(m_file)
		//	, message.c_str()				
		//	);
		//}
		Holder<Logger>::hostage()
			->logMessage( message, m_level );

		//if( m_level == LM_ERROR )
		//{
		//	ScriptEngine * scriptEng = ScriptEngine::hostage();
		//	
		//	if( scriptEng )
		//	{
		//		scriptEng->writeError( "Logger internal error" );
		//	}
		//}

		/*if( ( m_options & LO_MESSAGE_BOX ) != 0 )
		{
			Holder<Logger>::hostage()
				->logMessage( message );
			Holder<Application>::hostage()->showMessageBox( message, "Mengine Critical Error", 0 );
		}*/

	}
	//////////////////////////////////////////////////////////////////////////
	/*Log::Log()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Log::~Log()
	{
		os << std::endl;
		Holder<Logger>::hostage()->logMessage( os.str(), m_level );
	}
	//////////////////////////////////////////////////////////////////////////
	std::ostringstream& Log::get( EMessageLevel level  = LM_LOG )
	{
		m_level = level;
		os << Log::time() << ": ";
		return os;
	}
	//////////////////////////////////////////////////////////////////////////
	String Log::time()
	{
		std::time_t ctTime;
		std::time( &ctTime );
		std::tm* sTime = std::localtime( &ctTime );
		char result[9] = {0};
		sprintf_s( result, 9, "%02d:%02d:%02d", sTime->tm_hour, sTime->tm_min, sTime->tm_sec );
		return result;
	}*/
	//////////////////////////////////////////////////////////////////////////
}
