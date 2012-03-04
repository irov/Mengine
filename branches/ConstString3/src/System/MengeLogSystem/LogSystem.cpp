//#	include "LogSystem.h"
//
//#	include "Interface/FileSystemInterface.h"
//#	include "Interface/LogSystemInterface.h"
//#	include <algorithm>
//
////////////////////////////////////////////////////////////////////////////
//bool initInterfaceSystem( Menge::LogSystemInterface** _interface )
//{
//	if( _interface == 0 )
//	{
//		return false;
//	}
//	
//	*_interface = new MengeLogSystem();
//	
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//void releaseInterfaceSystem( Menge::LogSystemInterface* _interface )
//{
//	if( _interface )
//	{
//		delete static_cast<MengeLogSystem*>( _interface );
//	}
//}
////////////////////////////////////////////////////////////////////////////
//MengeLogSystem::MengeLogSystem()
//: m_verboseLevel( Menge::LM_ERROR )
//{
//}
////////////////////////////////////////////////////////////////////////////
//MengeLogSystem::~MengeLogSystem()
//{
//	for( TVectorLoggers::iterator 
//		it = m_loggers.begin(), 
//		it_end = m_loggers.end();
//	it != it_end;
//	++it )
//	{
//		(*it)->flush();
//	}
//}
////////////////////////////////////////////////////////////////////////////
//void MengeLogSystem::setVerboseLevel( Menge::EMessageLevel _level )
//{
//	m_verboseLevel = _level;
//}
////////////////////////////////////////////////////////////////////////////
//bool MengeLogSystem::validVerboseLevel( Menge::EMessageLevel _level ) const
//{
//	if( m_verboseLevel < _level )
//	{
//		return false;
//	}
//
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//void MengeLogSystem::logMessage( Menge::EMessageLevel _level, const Menge::String& _message )
//{
//	for( TVectorLoggers::iterator 
//		it = m_loggers.begin(), 
//		it_end = m_loggers.end();
//	it != it_end;
//	++it )
//	{
//		(*it)->log( _message.c_str(), _message.size(), _level );
//	}
//}
////////////////////////////////////////////////////////////////////////////
//bool MengeLogSystem::registerLogger( Menge::LoggerInterface* _logger )
//{
//	TVectorLoggers::iterator it_find = std::find( m_loggers.begin(), m_loggers.end(), _logger );
//	if( it_find != m_loggers.end() )
//	{
//		return false;
//	}
//
//	m_loggers.push_back( _logger );
//
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//void MengeLogSystem::unregisterLogger( Menge::LoggerInterface* _logger )
//{
//	TVectorLoggers::iterator it_find = std::find( m_loggers.begin(), m_loggers.end(), _logger );
//	if( it_find == m_loggers.end() )
//	{
//		return;
//	}
//
//	_logger->flush();
//	
//	m_loggers.erase( it_find );
//}
////////////////////////////////////////////////////////////////////////////
