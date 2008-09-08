#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class OutStreamInterface;

	class LogSystemInterface
	{
	public:
		virtual bool initialize( OutStreamInterface* _logStream ) = 0;
		virtual void enableConsole( bool _console ) = 0;
		virtual void logMessage( const String& _message, bool _maskDebug = false, bool _endl = true, bool _timeStamp = true ) = 0;
	};
}

bool	initInterfaceSystem( Menge::LogSystemInterface** );
void	releaseInterfaceSystem( Menge::LogSystemInterface* );