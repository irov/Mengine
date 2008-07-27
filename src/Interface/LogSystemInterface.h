#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class LogSystemInterface
	{
	public:
		virtual void startLog( const String& _filename ) = 0;
		virtual void logMessage( const String& _message, bool _maskDebug = false, bool _endl = true, bool _timeStamp = true ) = 0;
	};

}

bool	initInterfaceSystem( Menge::LogSystemInterface** );
void	releaseInterfaceSystem( Menge::LogSystemInterface* );