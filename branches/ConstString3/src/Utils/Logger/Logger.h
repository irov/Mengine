#	pragma once

#	include "Interface/LogSystemInterface.h"

namespace Menge
{	
	class LoggerOperator
	{
	public:
		LoggerOperator( LogSystemInterface * _logger, const char * _file, EMessageLevel _level );

	public:
		void operator()( const char* _message, ... );

	protected:
		LogSystemInterface * m_log;

		const char * m_file;
		EMessageLevel m_level;
	};
}

#	define LOGGER_ERROR( LOGGER )\
	Menge::LoggerOperator( LOGGER, __FILE__, Menge::LM_ERROR )
	//Menge::Log().get( Menge::LM_ERROR )

#	define LOGGER_WARNING( LOGGER )\
	Menge::LoggerOperator( LOGGER, __FILE__, Menge::LM_WARNING )
	//Menge::Log().get( Menge::LM_WARNING )

#	define LOGGER_INFO( LOGGER )\
	Menge::LoggerOperator( LOGGER, __FILE__, Menge::LM_INFO)
