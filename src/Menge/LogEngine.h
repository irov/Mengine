#	pragma once

#	include "Interface/LogSystemInterface.h"

#	include "Holder.h"

#	include "TextField.h"

#	include <string>

#	include <map>

namespace Menge
{
	class LogEngine
	{
	public:
		LogEngine( LogSystemInterface * _interface );

	public:
		void logMessage( const String& _message, int _messageLevel, bool _endl = true, bool _timeStamp = true );
		void setVerboseLevel( int _level );
		LogSystemInterface* getInterface();
		
	protected:
		LogSystemInterface * m_interface;
	};

	enum EMessageLevel
	{
		LM_ERROR = 0,
		LM_LOG = 1,
		LM_INFO = 2
	};

	enum ELogOption
	{
		LO_TEXT = 0x00000000,
		LO_CONSOLE = 0x00000001,
		LO_SCRIPT_BREAK = 0x00000002,
		LO_MESSAGE_BOX = 0x00000004
	};

	class LoggerOperator
	{
	public:
		LoggerOperator( const char * _file, int _level, unsigned int _options );

	public:
		void operator()( const TChar* _message, ... );

	protected:
		//print_( const TChar* _message );
		const char * m_file;
		int m_level;
		unsigned int m_options;
	};
}

#	define MENGE_LOG_ERROR\
	Menge::LoggerOperator( __FILE__, LM_ERROR, LO_TEXT )

#	define MENGE_LOG\
	Menge::LoggerOperator( __FILE__, LM_LOG, LO_TEXT )

//#	define MENGE_LOG_DEBUG\
//	Menge::LoggerOperator( __FILE__, ELoggerLog | ELoggerDebug )

#	define MENGE_SCRIPT_BREACK\
	Menge::LoggerOperator( __FILE__, LM_ERROR, LO_SCRIPT_BREAK )

#	define MENGE_LOG_CRITICAL\
	Menge::LoggerOperator( __FILE__, LM_ERROR, LO_MESSAGE_BOX )
