#	pragma once

class LogSystemInterface
{
public:
	virtual void startLog( const char* _filename ) = 0;
	virtual void logMessage( const char * _message, bool _maskDebug ) = 0;
};

bool	initInterfaceSystem( LogSystemInterface** );
void	releaseInterfaceSystem( LogSystemInterface* );