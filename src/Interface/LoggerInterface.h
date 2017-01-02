#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	//class OutputStreamInterface;
	enum EMessageLevel
	{
		LM_FATAL = 0,
		LM_CRITICAL = 1,
		LM_ERROR = 2,
		LM_PERFOMANCE = 3,
		LM_WARNING = 4,
		LM_LOG = 5,
		LM_INFO = 6,

		LM_MAX = 7
	};

	class LoggerInterface
        : public ServantInterface
	{
	public:
		LoggerInterface()
		{
		};

		virtual ~LoggerInterface()
		{
		};

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual void setVerboseLevel( EMessageLevel _level ) = 0;
        virtual void setVerboseFlag( uint32_t _flag ) = 0;

    public:
		virtual bool validMessage( EMessageLevel _level, uint32_t _flag ) const = 0;

	public:
		virtual void log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _size ) = 0;
		virtual void flush() = 0;
	};
	
	class LoggerServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE( "LoggerService" )

	public:
		virtual void setVerboseLevel( EMessageLevel _level ) = 0;
        virtual void setVerboseFlag( uint32_t _flag ) = 0;

    public:
		virtual bool validMessage( EMessageLevel _level, uint32_t _flag ) const = 0;

	public:
		virtual void logMessage( Menge::EMessageLevel _level, uint32_t _flag, const char * _message, size_t _size  ) = 0;
		virtual size_t getCountMessage( Menge::EMessageLevel _level ) = 0;

	public:
		virtual bool registerLogger( LoggerInterface* _logger ) = 0;
		virtual void unregisterLogger( LoggerInterface* _logger ) = 0;
	};
    
#   define LOGGER_SERVICE( serviceProvider )\
    ((Menge::LoggerServiceInterface*)SERVICE_GET(serviceProvider, Menge::LoggerServiceInterface))
}

