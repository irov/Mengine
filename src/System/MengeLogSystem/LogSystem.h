#	pragma once

#	include "Interface/LogSystemInterface.h"
//#	include "Interface/FileSystemInterface.h"
#	include <fstream>

class MengeLogSystem
	: public LogSystemInterface
{
public:
	MengeLogSystem();
	~MengeLogSystem();

	void startLog( const char* _filename ) override;

	void logMessage( const char * _message, bool _maskDebug ) override;

private:
	std::ofstream m_logStream;
};
