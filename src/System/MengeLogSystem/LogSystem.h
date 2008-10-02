#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include <fstream>

class MengeLogSystem
	: public Menge::LogSystemInterface
{
public:
	MengeLogSystem();
	~MengeLogSystem();

	void setVerboseLevel( Menge::EMessageLevel _level ) override;
	void logMessage( const Menge::StringA& _message, Menge::EMessageLevel _level = Menge::LM_LOG ) override;
	bool registerLogger( Menge::LoggerInterface* _logger ) override;
	void unregisterLogger( Menge::LoggerInterface* _logger ) override;
	
private:
	std::ofstream m_logStream;
	Menge::EMessageLevel m_verboseLevel;
	typedef std::vector<Menge::LoggerInterface*> TVectorLoggers;
	TVectorLoggers m_loggers;
};
