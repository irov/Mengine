#	include "LogSystem.h"

#	include <algorithm>

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
	Menge::StringA filename( "Menge.log" );
	std::locale loc("");
	m_logStream.imbue( loc );
	m_logStream.open( filename.c_str() );
	if( m_logStream.fail() )
	{
	}
}
//////////////////////////////////////////////////////////////////////////
MengeLogSystem::~MengeLogSystem()
{
	m_logStream.close();
}
//////////////////////////////////////////////////////////////////////////
void MengeLogSystem::setVerboseLevel( Menge::EMessageLevel _level )
{
	m_verboseLevel = _level;
}
//////////////////////////////////////////////////////////////////////////
void MengeLogSystem::logMessage( const Menge::StringA& _message, Menge::EMessageLevel _level /* = LM_LOG */ )
{
	if( m_verboseLevel < _level ) 
	{
		return;
	}
	m_logStream	<< _message;

	m_logStream.flush();
	for( TVectorLoggers::iterator it = m_loggers.begin(), it_end = m_loggers.end();
		it != it_end;
		it++ )
	{
		(*it)->outputMessage( _message );
	}
}
//////////////////////////////////////////////////////////////////////////
bool MengeLogSystem::registerLogger( Menge::LoggerInterface* _logger )
{
	TVectorLoggers::iterator it_find = std::find( m_loggers.begin(), m_loggers.end(), _logger );
	if( it_find != m_loggers.end() )
	{
		return false;
	}
	m_loggers.push_back( _logger );
	return true;
}
//////////////////////////////////////////////////////////////////////////
void MengeLogSystem::unregisterLogger( Menge::LoggerInterface* _logger )
{
	TVectorLoggers::iterator it_find = std::find( m_loggers.begin(), m_loggers.end(), _logger );
	if( it_find != m_loggers.end() )
	{
		m_loggers.erase( it_find );
	}
}
//////////////////////////////////////////////////////////////////////////