#	pragma once

#	include "Interface/LogSystemInterface.h"

#	include <Utils/Core/Holder.h>

#	include <Logger/Logger.h>

#	include <vector>

namespace Menge
{
	class LogEngine
		: public Holder<LogEngine>
		, public LogServiceInterface
	{
	public:
		LogEngine();
		~LogEngine();

	public:
		void setVerboseLevel( EMessageLevel _level ) override;
		bool validVerboseLevel( EMessageLevel _level ) const override;

	public:
		void logMessage( EMessageLevel _level, const String& _message ) override;

	public:
		bool registerLogger( LoggerInterface* _logger ) override;
		void unregisterLogger( LoggerInterface* _logger ) override;
	
	protected:
		Menge::EMessageLevel m_verboseLevel;

		typedef std::vector<Menge::LoggerInterface*> TVectorLoggers;
		TVectorLoggers m_loggers;
	};
}

#	define MENGE_LOG_ERROR\
	LOGGER_ERROR( LogEngine::get() )

#	define MENGE_LOG_WARNING\
	LOGGER_WARNING( LogEngine::get() )

#	define MENGE_LOG_INFO\
	LOGGER_INFO( LogEngine::get() )