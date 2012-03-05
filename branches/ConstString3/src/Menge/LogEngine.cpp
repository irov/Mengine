#	include "LogEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LogEngine::LogEngine()
		: m_verboseLevel(LM_ERROR)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	LogEngine::~LogEngine()
	{
		for( TVectorLoggers::iterator 
			it = m_loggers.begin(), 
			it_end = m_loggers.end();
		it != it_end;
		++it )
		{
			(*it)->flush();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LogEngine::validVerboseLevel( EMessageLevel _level ) const
	{
		if( m_verboseLevel < _level )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::logMessage( EMessageLevel _level, const String& _message )
	{
		for( TVectorLoggers::iterator 
			it = m_loggers.begin(), 
			it_end = m_loggers.end();
		it != it_end;
		++it )
		{
			LoggerInterface * logger = (*it);

			if( logger->validVerboseLevel( _level ) == false )
			{
				continue;
			}

			logger->log( _message.c_str(), _message.size(), _level );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool LogEngine::registerLogger( LoggerInterface* _logger )
	{
		TVectorLoggers::iterator it_find = 
			std::find( m_loggers.begin(), m_loggers.end(), _logger );

		if( it_find != m_loggers.end() )
		{
			return false;
		}

		m_loggers.push_back( _logger );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::unregisterLogger( LoggerInterface* _logger )
	{
		TVectorLoggers::iterator it_find = 
			std::find( m_loggers.begin(), m_loggers.end(), _logger );

		if( it_find == m_loggers.end() )
		{
			return;
		}

		_logger->flush();

		m_loggers.erase( it_find );
	}
}