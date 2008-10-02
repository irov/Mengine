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
	bool registerLogger( Menge::LoggerInterface* _logger ) override;
	void unregisterLogger( Menge::LoggerInterface* _logger ) override;
	
private:
	std::ofstream m_logStream;
	bool m_error;

	typedef std::vector<Menge::LoggerInterface*> TVectorLoggers;
	TVectorLoggers m_loggers;
};
