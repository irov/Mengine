#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include <Utils/Core/Holder.h>

#	include <Logger/Logger.h>

namespace Menge
{
	class LogEngine
		: public Holder<LogEngine>
		, public LogServiceInterface
	{
	public:
		LogEngine( LogSystemInterface * _interface );
		~LogEngine();

	public:
		void setVerboseLevel( EMessageLevel _level );
		bool registerLogger( LoggerInterface* _logger );
		void unregisterLogger( LoggerInterface* _logger );
	
	public:
		LogSystemInterface* getInterface() override;
		
	protected:
		LogSystemInterface * m_interface;
	};
}

#	define MENGE_LOG_ERROR\
	LOGGER_ERROR( LogEngine::get()->getInterface() )

#	define MENGE_LOG_WARNING\
	LOGGER_WARNING( LogEngine::get()->getInterface() )

#	define MENGE_LOG_INFO\
	LOGGER_INFO( LogEngine::get()->getInterface() )