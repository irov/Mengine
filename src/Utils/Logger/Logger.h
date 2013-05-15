#	pragma once

#	include "Interface/LogSystemInterface.h"

namespace Menge
{	
	class LoggerOperator
	{
	public:
		LoggerOperator( LogServiceInterface * _logger, EMessageLevel _level, size_t _flag );

	public:
		LoggerOperator & operator()( const char* _message, ... );

	public:
		void logMessage(  const char * _message, size_t _size  );

	protected:
		LogServiceInterface * m_log;
		EMessageLevel m_level;
        size_t m_flag;
	};
}

#	define LOGGER_VERBOSE_LEVEL( LOGGER, LEVEL )\
	if( LOGGER->validMessage(LEVEL, 0) == false) {} else Menge::LoggerOperator(LOGGER, LEVEL, 0)

#	define LOGGER_ERROR( serviceProvider )\
	LOGGER_VERBOSE_LEVEL( LOG_SERVICE(serviceProvider), Menge::LM_ERROR )
	//if( LOGGER->validVerboseLevel(Menge::LM_ERROR) == false) {} else Menge::LoggerOperator( LOGGER, 1 )
		
#	define LOGGER_WARNING( serviceProvider )\
	LOGGER_VERBOSE_LEVEL( LOG_SERVICE(serviceProvider), Menge::LM_WARNING )
	
#	define LOGGER_INFO( serviceProvider )\
	LOGGER_VERBOSE_LEVEL( LOG_SERVICE(serviceProvider), Menge::LM_INFO )

