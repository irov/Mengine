#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class OutStreamInterface;

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

		virtual bool initialize( OutStreamInterface* _logStream ) = 0;
		virtual void logMessage( const StringA& _message ) = 0;
		virtual bool registerLogger( LoggerInterface* _logger ) = 0;
		virtual void unregisterLogger( LoggerInterface* _logger ) = 0;
	};
}

bool	initInterfaceSystem( Menge::LogSystemInterface** );
void	releaseInterfaceSystem( Menge::LogSystemInterface* );