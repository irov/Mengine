#	include "OgreLogSystem.h"

#	include <iostream>
#	include <time.h>
#	include <iomanip>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( LogSystemInterface** _interface )
{
	if( _interface == 0 )
	{
		return false;
	}
	
	*_interface = new OgreLogSystem();
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( LogSystemInterface* _interface )
{
	if( _interface )
	{
		delete _interface;
	}
}
//////////////////////////////////////////////////////////////////////////
OgreLogSystem::OgreLogSystem()
{
}
//////////////////////////////////////////////////////////////////////////
OgreLogSystem::~OgreLogSystem()
{
	m_logStream.close();
}
//////////////////////////////////////////////////////////////////////////
void OgreLogSystem::startLog( const char* _filename )
{
	m_logStream.open( _filename );
}
//////////////////////////////////////////////////////////////////////////
void OgreLogSystem::logMessage( const char * _message, bool _maskDebug )
{
	if ( !_maskDebug )
		std::cerr << _message << std::endl;

	time_t ctTime; 
	time(&ctTime);
	tm sTime;
	localtime_s( &sTime, &ctTime );
	m_logStream << std::setw(2) << std::setfill('0') << sTime.tm_hour
		<< ":" << std::setw(2) << std::setfill('0') << sTime.tm_min
		<< ":" << std::setw(2) << std::setfill('0') << sTime.tm_sec 
		<< ": " << _message << std::endl;

	// Flush stcmdream to ensure it is written (incase of a crash, we need log to be up to date)
	m_logStream.flush();
}