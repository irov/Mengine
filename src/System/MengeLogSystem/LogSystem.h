#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include <fstream>
#	include <map>

class MengeLogSystem
	: public Menge::LogSystemInterface
{
public:
	MengeLogSystem();
	~MengeLogSystem();

	void startLog( const Menge::String& _filename ) override;
	void logMessage( const Menge::String& _message, bool _maskDebug, bool _endl, bool _timeStamp ) override;
	
private:

	std::ofstream m_logStream;

	bool m_error;
};
