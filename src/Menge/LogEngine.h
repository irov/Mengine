#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include "Holder.h"

#	include <string>

#	include <map>
#	include <sstream>

namespace Menge
{
	class LogEngine
	{
	public:
		LogEngine( LogSystemInterface * _interface );

	public:
		void logMessage( const StringA& _message );
		void setVerboseLevel( int _level );
		int getVerboseLevel() const;
		void enableConsole( bool _enable );
		LogSystemInterface* getInterface();
		
	protected:
		LogSystemInterface * m_interface;
		int m_verboseLevel;
		bool m_console;
	};

	enum EMessageLevel
	{
		LM_ERROR = 0,
		LM_WARNING = 1,
		LM_LOG = 2,
		LM_INFO = 3
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
		void operator()( const char* _message, ... );

	protected:
		//print_( const TChar* _message );
		const char * m_file;
		int m_level;
		unsigned int m_options;
	};

	class Log
	{
	public:
		Log();
		virtual ~Log();
		std::ostringstream& get(EMessageLevel level = LM_LOG);
		StringA time();
	protected:
		std::ostringstream os;
	private:
		Log(const Log&);
		Log& operator =(const Log&);
	};
}

#	define MENGE_LOG_ERROR\
	Menge::Log().get( Menge::LM_ERROR )

#	define MENGE_LOG_WARNING\
	if( Menge::LM_WARNING > Menge::Holder<Menge::LogEngine>::hostage()->getVerboseLevel() ) ;\
	else Menge::Log().get( Menge::LM_WARNING )

#	define MENGE_LOG\
	if( Menge::LM_LOG > Menge::Holder<Menge::LogEngine>::hostage()->getVerboseLevel() ) ;\
	else Menge::Log().get( Menge::LM_LOG )

//#	define MENGE_LOG_DEBUG\
//	Menge::LoggerOperator( __FILE__, ELoggerLog | ELoggerDebug )

#	define MENGE_SCRIPT_BREACK\
	Menge::LoggerOperator( __FILE__, LM_ERROR, LO_SCRIPT_BREAK )

#	define MENGE_LOG_CRITICAL\
	Menge::LoggerOperator( __FILE__, LM_ERROR, LO_MESSAGE_BOX )
