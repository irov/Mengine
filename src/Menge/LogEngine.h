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
		void logMessage( const std::string & _message, bool _maskDebug, bool _endl = true, bool _timeStamp = true );
		LogSystemInterface* getInterface();
		void displayStats( TextField * _debugTextField );

	protected:
		LogSystemInterface * m_interface;
	};

	enum ELoggerMask
	{
		ELoggerLog  = 0x00000000,
		ELoggerBreak = 0x00000001,
		ELoggerError = 0x00000002,
		ELoggerDebug = 0x00000004
	};

	class LoggerOperator
	{
	public:
		LoggerOperator( const char * _file, unsigned int _mask );

	public:
		void operator()( const char * _message, ... );

	protected:
		const char * m_file;
		unsigned int m_mask;
	};
}

#	define MENGE_LOG\
	Menge::LoggerOperator( __FILE__, ELoggerLog )

#	define MENGE_LOG_DEBUG\
	Menge::LoggerOperator( __FILE__, ELoggerLog | ELoggerDebug )

#	define MENGE_SCRIPT_BREACK\
	Menge::LoggerOperator( __FILE__, ELoggerLog | ELoggerBreak )
