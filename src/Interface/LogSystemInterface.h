#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class OutStreamInterface;

	enum EMessageLevel
	{
		LM_ERROR = 0,
		LM_WARNING = 1,
		LM_LOG = 2,
		LM_INFO = 3,

		LM_MAX
	};

	class LogSystemInterface
	{
	public:
		//virtual ~LogSystemInterface() {};

		virtual void setVerboseLevel( EMessageLevel _level ) = 0;
		virtual void logMessage( const StringA& _message, EMessageLevel _level = LM_LOG ) = 0;
		virtual bool registerLogger( OutStreamInterface* _logger ) = 0;
		virtual void unregisterLogger( OutStreamInterface* _logger ) = 0;
	};
}

bool	initInterfaceSystem( Menge::LogSystemInterface** );
void	releaseInterfaceSystem( Menge::LogSystemInterface* );