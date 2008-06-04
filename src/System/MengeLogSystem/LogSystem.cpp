#	include "LogSystem.h"

#	include <iostream>
#	include <ctime>
#	include <iomanip>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::LogSystemInterface** _interface )
{
	if( _interface == 0 )
	{
		return false;
	}
	
	*_interface = new MengeLogSystem();
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::LogSystemInterface* _interface )
{
	if( _interface )
	{
		delete static_cast<MengeLogSystem*>( _interface );
	}
}
//////////////////////////////////////////////////////////////////////////
MengeLogSystem::MengeLogSystem()
{
}
//////////////////////////////////////////////////////////////////////////
MengeLogSystem::~MengeLogSystem()
{
	m_logStream.close();
}
//////////////////////////////////////////////////////////////////////////
void MengeLogSystem::startLog( const Menge::String& _filename )
{
	m_logStream.open( _filename.c_str() );
}
//////////////////////////////////////////////////////////////////////////
void MengeLogSystem::logMessage( const Menge::String& _message, bool _maskDebug, bool _endl, bool _timeStamp )
{
	if ( !_maskDebug )
	{
		std::cerr << _message;

		if( _endl )
		{
			std::cerr << std::endl;
		}
	}

	std::time_t ctTime; 
	std::time(&ctTime);
	std::tm* sTime = std::localtime( &ctTime );
	if( _timeStamp )
	{
		m_logStream << std::setw(2) << std::setfill('0') << sTime->tm_hour
			<< ":" << std::setw(2) << std::setfill('0') << sTime->tm_min
			<< ":" << std::setw(2) << std::setfill('0') << sTime->tm_sec << ":";
	}
	m_logStream	<< _message;
	if( _endl )
	{
		m_logStream << std::endl;
	}

	// Flush stcmdream to ensure it is written (incase of a crash, we need log to be up to date)
	m_logStream.flush();
}