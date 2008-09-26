#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class OutStreamInterface;

	class LogSystemInterface
	{
	public:
		virtual bool initialize( OutStreamInterface* _logStream ) = 0;
		virtual void logMessage( const StringA& _message ) = 0;
	};
}

bool	initInterfaceSystem( Menge::LogSystemInterface** );
void	releaseInterfaceSystem( Menge::LogSystemInterface* );