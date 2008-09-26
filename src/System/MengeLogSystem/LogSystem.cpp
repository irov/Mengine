#	include "LogSystem.h"

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
	Menge::StringA filename( "Menge.log" );
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
void MengeLogSystem::logMessage( const Menge::StringA& _message )
{
	if( m_error ) return;

	m_logStream	<< _message;

	m_logStream.flush();
}
//////////////////////////////////////////////////////////////////////////