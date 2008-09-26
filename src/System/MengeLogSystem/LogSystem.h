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
	void logMessage( const Menge::StringA& _message ) override;
	
private:
	std::ofstream m_logStream;
	bool m_error;
};
