#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
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
		virtual ~LoggerInterface() {};

		virtual void outputMessage( const StringA& _message ) = 0;
	};

	class LogSystemInterface
	{
	public:
		virtual ~LogSystemInterface() {};

		virtual void setVerboseLevel( EMessageLevel _level ) = 0;
		virtual void logMessage( const StringA& _message, EMessageLevel _level = LM_LOG ) = 0;
		virtual bool registerLogger( LoggerInterface* _logger ) = 0;
		virtual void unregisterLogger( LoggerInterface* _logger ) = 0;
	};
}

bool	initInterfaceSystem( Menge::LogSystemInterface** );
void	releaseInterfaceSystem( Menge::LogSystemInterface* );