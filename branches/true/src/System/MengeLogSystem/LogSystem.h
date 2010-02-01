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
	void logMessage( const Menge::String& _message, Menge::EMessageLevel _level = Menge::LM_LOG ) override;
	bool registerLogger( Menge::OutputStreamInterface* _logger ) override;
	void unregisterLogger( Menge::OutputStreamInterface* _logger ) override;
	
private:
	std::ofstream m_logStream;
	Menge::EMessageLevel m_verboseLevel;
	typedef std::vector<Menge::OutputStreamInterface*> TVectorLoggers;
	TVectorLoggers m_loggers;
};
