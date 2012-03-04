#	pragma once

#	include "Interface/LogSystemInterface.h"

namespace Menge
{	
	class LoggerOperator
	{
	public:
		LoggerOperator( LogServiceInterface * _logger, EMessageLevel _level );

	public:
		void operator()( const char* _message, ... );

	public:
		void logMessage( const std::string & _message );

	protected:
		LogServiceInterface * m_log;
		EMessageLevel m_level;
	};
}

#	define LOGGER_VERBOSE_LEVEL( LOGGER, LEVEL )\
	if( LOGGER->validVerboseLevel(LEVEL) == false) {} else Menge::LoggerOperator(LOGGER, LEVEL)

#	define LOGGER_ERROR( LOGGER )\
	LOGGER_VERBOSE_LEVEL( LOGGER, Menge::LM_ERROR )
	//if( LOGGER->validVerboseLevel(Menge::LM_ERROR) == false) {} else Menge::LoggerOperator( LOGGER, 1 )
		
#	define LOGGER_WARNING( LOGGER )\
	LOGGER_VERBOSE_LEVEL( LOGGER, Menge::LM_WARNING )
	
#	define LOGGER_INFO( LOGGER )\
	LOGGER_VERBOSE_LEVEL( LOGGER, Menge::LM_INFO )

