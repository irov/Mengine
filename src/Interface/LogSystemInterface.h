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
		virtual void logMessage( const String& _message, int _messageLevel = 1, bool _endl = true, bool _timeStamp = true ) = 0;
		virtual void setVerboseLevel( int _level ) = 0;
	};
}

bool	initInterfaceSystem( Menge::LogSystemInterface** );
void	releaseInterfaceSystem( Menge::LogSystemInterface* );