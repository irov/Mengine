#pragma once

#include "Interface/LoggerInterface.h"

namespace Mengine
{	
	class LoggerOperator
	{
	public:
		LoggerOperator( LoggerServiceInterface * _logger, EMessageLevel _level, uint32_t _flag );

	public:
		LoggerOperator & operator()( const Char * _format, ... );

	public:
		void logMessage( const Char * _format, uint32_t _size );

	protected:
		LoggerServiceInterface * m_log;
		EMessageLevel m_level;
        uint32_t m_flag;
	};
}

#	define LOGGER_VERBOSE_LEVEL( LOGGER, LEVEL )\
	if( LOGGER->validMessage(LEVEL, 0) == false) {} else Mengine::LoggerOperator(LOGGER, LEVEL, 0)

#	define LOGGER_FATAL\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(), Mengine::LM_FATAL )

#	define LOGGER_CRITICAL\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(), Mengine::LM_CRITICAL )

#	define LOGGER_ERROR\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(), Mengine::LM_ERROR )

#	define LOGGER_PERFORMANCE\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(), Mengine::LM_PERFOMANCE )

#	define LOGGER_STATISTIC\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(), Mengine::LM_STATISTIC )

#	define LOGGER_WARNING\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(), Mengine::LM_WARNING )
	
#	define LOGGER_INFO\
	LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(), Mengine::LM_INFO )

