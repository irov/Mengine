#	pragma once

#	include "Interface/LogSystemInterface.h"

class OgreLogSystem
	: public LogSystemInterface
{
public:
	void logMessage( const char * _message, bool _maskDebug );
};
