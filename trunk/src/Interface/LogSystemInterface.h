#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/PluginInterface.h"

namespace Menge
{
	//class OutputStreamInterface;
	enum EMessageLevel
	{
		LM_ERROR = 0,
		LM_WARNING = 1,
		LM_LOG = 2,
		LM_INFO = 3,

		LM_MAX
	};

	class LoggerInterface
	{
	public:
		virtual void setVerboseLevel( EMessageLevel _level ) = 0;
		virtual bool validVerboseLevel( EMessageLevel _level ) const = 0;

	public:
		virtual void log( const char * _data, int _count, EMessageLevel _level ) = 0;
		virtual void flush() = 0;
	};
	
	class LogServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE( "LogService" )

	public:
		virtual void setVerboseLevel( EMessageLevel _level ) = 0;
		virtual bool validVerboseLevel( EMessageLevel _level ) const = 0;

	public:
		virtual void logMessage( Menge::EMessageLevel _level,  const char * _message, size_t _size  ) = 0;

	public:
		virtual bool registerLogger( LoggerInterface* _logger ) = 0;
		virtual void unregisterLogger( LoggerInterface* _logger ) = 0;
	};
    
#   define LOG_SERVICE( serviceProvider )\
    (Menge::getService<Menge::LogServiceInterface>(serviceProvider))
}

