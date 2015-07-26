#	include "LogEngine.h"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LogService, Menge::LogServiceInterface, Menge::LogEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LogEngine::LogEngine()
		: m_verboseLevel(LM_ERROR)
        , m_serviceProvider(nullptr)
        , m_verboseFlag(0)
	{
		for( size_t i = 0; i != LM_MAX; ++i )
		{
			m_countMessage[i] = 0;
		}		
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
    void LogEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * LogEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
    //////////////////////////////////////////////////////////////////////////
    void LogEngine::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
	//////////////////////////////////////////////////////////////////////////
	bool LogEngine::validMessage( EMessageLevel _level, uint32_t _flag ) const
	{
		if( m_verboseLevel < _level )
		{
			return false;
		}

        if( _flag == 0 )
        {
            return true;
        }

        if( (m_verboseFlag & _flag) == 0 )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::logMessage( EMessageLevel _level, uint32_t _flag, const char * _message, size_t _size  )
	{
		++m_countMessage[_level];

		for( TVectorLoggers::iterator 
			it = m_loggers.begin(), 
			it_end = m_loggers.end();
		it != it_end;
		++it )
		{
			LoggerInterface * logger = (*it);

			if( logger->validMessage( _level, _flag ) == false )
			{
				continue;
			}

			logger->log( _level, _flag, _message, _size );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t LogEngine::getCountMessage( Menge::EMessageLevel _level )
	{
		size_t count = m_countMessage[_level];

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LogEngine::registerLogger( LoggerInterface * _logger )
	{
		TVectorLoggers::iterator it_find = 
			std::find( m_loggers.begin(), m_loggers.end(), _logger );

		if( it_find != m_loggers.end() )
		{
			return false;
		}

		_logger->setServiceProvider( m_serviceProvider );

		if( _logger->initialize() == false )
		{
			return false;
		}

		m_loggers.push_back( _logger );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void LogEngine::unregisterLogger( LoggerInterface * _logger )
	{
		TVectorLoggers::iterator it_find = 
			std::find( m_loggers.begin(), m_loggers.end(), _logger );

		if( it_find == m_loggers.end() )
		{
			return;
		}

		_logger->flush();

		_logger->finalize();

		m_loggers.erase( it_find );
	}
}