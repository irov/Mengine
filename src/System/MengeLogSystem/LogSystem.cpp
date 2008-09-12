#	include "LogSystem.h"

#	include <ctime>
#	include <iomanip>
#	include <algorithm>
#	include <assert.h>
#	include <locale.h>
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
: m_error( false )
, m_console( false )
, m_verboseLevel( 0 )
{
}
//////////////////////////////////////////////////////////////////////////
MengeLogSystem::~MengeLogSystem()
{
	if( m_error == false )
	{
		m_logStream.close();
	}
}
//////////////////////////////////////////////////////////////////////////
bool MengeLogSystem::initialize( Menge::OutStreamInterface* _logStream )
{
	Menge::String filename( MENGE_TEXT("Menge.log") );
	std::locale loc("");
	m_logStream.imbue( loc );
	m_logStream.open( filename.c_str() );
	if( m_logStream.fail() )
	{
		m_error = true;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
void MengeLogSystem::logMessage( const Menge::String& _message, int _messageLevel, bool _endl, bool _timeStamp )
{
	if( _messageLevel > m_verboseLevel )
	{
		return;
	}

	if ( m_console )
	{
		//std::cerr << _message;
		StdErr << _message;

		if( _endl )
		{
			//std::cerr << std::endl;
			StdErr << std::endl;
		}
	}

	if( m_error ) return;

	std::time_t ctTime; 
	std::time(&ctTime);
	std::tm * sTime = std::localtime( &ctTime );
	if( _timeStamp )
	{
		m_logStream << std::setw(2) << std::setfill(MENGE_TEXT('0')) << sTime->tm_hour
			<< MENGE_TEXT(":") << std::setw(2) << std::setfill(MENGE_TEXT('0')) << sTime->tm_min
			<< MENGE_TEXT(":") << std::setw(2) << std::setfill(MENGE_TEXT('0')) << sTime->tm_sec << MENGE_TEXT(":");
	}
	m_logStream	<< _message;
	if( _endl )
	{
		m_logStream << std::endl;
	}

	m_logStream.flush();
}
//////////////////////////////////////////////////////////////////////////
void MengeLogSystem::enableConsole( bool _console )
{
	m_console = _console;
}
//////////////////////////////////////////////////////////////////////////
void MengeLogSystem::setVerboseLevel( int _level )
{
	m_verboseLevel = _level;
}
//////////////////////////////////////////////////////////////////////////