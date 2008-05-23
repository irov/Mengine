#	pragma once

#	include "Interface/LogSystemInterface.h"
//#	include "Interface/FileSystemInterface.h"
#	include <fstream>

class OgreLogSystem
	: public LogSystemInterface
{
public:
	OgreLogSystem();
	~OgreLogSystem();

	void startLog( const char* _filename ) override;

	void logMessage( const char * _message, bool _maskDebug ) override;

private:
	std::ofstream m_logStream;
};
