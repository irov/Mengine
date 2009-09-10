#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include "Holder.h"

namespace Menge
{
	class LogEngine
		: public Holder<LogEngine>
	{
	public:
		LogEngine( LogSystemInterface * _interface );

	public:
		void logMessage( const String& _message, EMessageLevel _level = LM_LOG );
		void setVerboseLevel( EMessageLevel _level );
		LogSystemInterface* getInterface();
		
	protected:
		LogSystemInterface * m_interface;
	};

	/*enum ELogOption
	{
		LO_TEXT = 0x00000000,
		LO_CONSOLE = 0x00000001,
		LO_SCRIPT_BREAK = 0x00000002,
		LO_MESSAGE_BOX = 0x00000004
	};*/

	class LoggerOperator
	{
	public:
		LoggerOperator( const char * _file, EMessageLevel _level );
		void operator()( const char* _message, ... );

	protected:
		const char * m_file;
		EMessageLevel m_level;
	};

	/*class Log
	{
	public:
		Log();
		virtual ~Log();
		std::ostringstream& get(EMessageLevel level = LM_LOG);
		String time();
	protected:
		std::ostringstream os;
		EMessageLevel m_level;
	private:
		Log(const Log&);
		Log& operator =(const Log&);
	};*/
}

#	define MENGE_LOG_ERROR\
	Menge::LoggerOperator( __FILE__, Menge::LM_ERROR )
	//Menge::Log().get( Menge::LM_ERROR )

#	define MENGE_LOG_WARNING\
	Menge::LoggerOperator( __FILE__, Menge::LM_WARNING )
	//Menge::Log().get( Menge::LM_WARNING )

#	define MENGE_LOG\
	Menge::LoggerOperator( __FILE__, Menge::LM_LOG )
	//Menge::Log().get( Menge::LM_LOG )

//#	define MENGE_LOG_DEBUG\
//	Menge::LoggerOperator( __FILE__, ELoggerLog | ELoggerDebug )

/*#	define MENGE_SCRIPT_BREACK\
	Menge::LoggerOperator( __FILE__, LM_ERROR, LO_SCRIPT_BREAK )

#	define MENGE_LOG_CRITICAL\
	Menge::LoggerOperator( __FILE__, LM_ERROR, LO_MESSAGE_BOX )*/
