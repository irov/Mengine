#	pragma once

#	include "Interface/LogSystemInterface.h"

#	include <Logger/Logger.h>

#	include <stdex/stl_vector.h>

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
        void setVerboseFlag( size_t _flag ) override;

    public:
		bool validMessage( EMessageLevel _level, size_t _flag ) const override;

	public:
		void logMessage( EMessageLevel _level, size_t _flag, const char * _message, size_t _size  ) override;

	public:
		bool registerLogger( LoggerInterface* _logger ) override;
		void unregisterLogger( LoggerInterface* _logger ) override;
	
	protected:
        ServiceProviderInterface * m_serviceProvider;

		Menge::EMessageLevel m_verboseLevel;
        size_t m_verboseFlag;

		typedef stdex::vector<LoggerInterface *> TVectorLoggers;
		TVectorLoggers m_loggers;
	};
}