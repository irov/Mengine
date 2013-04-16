#	pragma once

#	include "Interface/LogSystemInterface.h"

#	include <Logger/Logger.h>

#	include <vector>

namespace Menge
{
	class LogEngine
		: public LogServiceInterface
	{
	public:
		LogEngine();
		~LogEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		void setVerboseLevel( EMessageLevel _level ) override;
		bool validVerboseLevel( EMessageLevel _level ) const override;

	public:
		void logMessage( EMessageLevel _level,  const char * _message, size_t _size  ) override;

	public:
		bool registerLogger( LoggerInterface* _logger ) override;
		void unregisterLogger( LoggerInterface* _logger ) override;
	
	protected:
        ServiceProviderInterface * m_serviceProvider;

		Menge::EMessageLevel m_verboseLevel;

		typedef std::vector<LoggerInterface*> TVectorLoggers;
		TVectorLoggers m_loggers;
	};
}