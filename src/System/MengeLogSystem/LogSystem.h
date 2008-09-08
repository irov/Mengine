#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include <fstream>

class MengeLogSystem
	: public Menge::LogSystemInterface
{
public:
	MengeLogSystem();
	~MengeLogSystem();

	bool initialize( Menge::OutStreamInterface* _logStream ) override;
	void enableConsole( bool _console ) override;
	void logMessage( const Menge::String& _message, bool _maskDebug, bool _endl, bool _timeStamp ) override;
	
private:

	Menge::StdOfstream m_logStream;

	bool m_error;
	bool m_console;
};
