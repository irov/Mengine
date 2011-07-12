#	include "LogEngine.h"

#	include <ctime>
#	include <cstdio>
#	include <stdarg.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LogEngine::LogEngine( LogSystemInterface * _interface )
		: m_interface(_interface)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	LogEngine::~LogEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	LogSystemInterface * LogEngine::getInterface()
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::setVerboseLevel( EMessageLevel _level )
	{
		m_interface->setVerboseLevel( _level );
	}
	//////////////////////////////////////////////////////////////////////////
	bool LogEngine::registerLogger( LoggerInterface* _logger )
	{
		return m_interface->registerLogger( _logger );
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::unregisterLogger( LoggerInterface* _logger )
	{
		m_interface->unregisterLogger( _logger );
	}
}