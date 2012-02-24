#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include <fstream>

class MengeLogSystem
	: public Menge::LogSystemInterface
{
public:
	MengeLogSystem();
	~MengeLogSystem();

protected:
	void setVerboseLevel( Menge::EMessageLevel _level ) override;
	bool validVerboseLevel( Menge::EMessageLevel _level ) const override;
	void logMessage( Menge::EMessageLevel _level, const Menge::String& _message ) override;
	bool registerLogger( Menge::LoggerInterface* _logger ) override;
	void unregisterLogger( Menge::LoggerInterface* _logger ) override;
	
private:
	std::ofstream m_logStream;
	Menge::EMessageLevel m_verboseLevel;
	typedef std::vector<Menge::LoggerInterface*> TVectorLoggers;
	TVectorLoggers m_loggers;
};
