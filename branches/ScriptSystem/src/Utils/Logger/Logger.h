#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include "Core/Holder.h"

namespace Menge
{
	class Logger
		: public Holder<Logger>
	{
	public:
		Logger();
		~Logger();

	public:
		bool initialize( LogSystemInterface * _interface );

	public:
		void logMessage( const String& _message, EMessageLevel _level = LM_LOG );
		void setVerboseLevel( EMessageLevel _level );
		bool registerLogger( OutputStreamInterface* _logger );
		void unregisterLogger( OutputStreamInterface* _logger );
		LogSystemInterface* getInterface();
		
	protected:
		LogSystemInterface * m_interface;
	};

	class LoggerOperator
	{
	public:
		LoggerOperator( const char * _file, EMessageLevel _level );

	public:
		void operator()( const char* _message, ... );

	protected:
		const char * m_file;
		EMessageLevel m_level;
	};
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
