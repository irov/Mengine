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
		virtual void log( const void* _data, int _count, EMessageLevel _level ) = 0;
		virtual void flush() = 0;
	};
	
	class LogSystemInterface
	{
	public:
		virtual void setVerboseLevel( EMessageLevel _level ) = 0;
		virtual bool validVerboseLevel( EMessageLevel _level ) = 0;
		virtual void logMessage( const String& _message, EMessageLevel _level = LM_LOG ) = 0;
		virtual bool registerLogger( LoggerInterface* _logger ) = 0;
		virtual void unregisterLogger( LoggerInterface* _logger ) = 0;
	};

	class LogServiceInterface
		: public ServiceInterface
	{
	public:
		virtual LogSystemInterface* getInterface() = 0;
	};
}

bool	initInterfaceSystem( Menge::LogSystemInterface** );
void	releaseInterfaceSystem( Menge::LogSystemInterface* );
