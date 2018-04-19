#include "LoggerEngine.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LoggerService, Mengine::LoggerEngine );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	LoggerEngine::LoggerEngine()
		: m_verboseLevel(LM_ERROR)
        , m_verboseFlag(0)
	{
		for( uint32_t i = 0; i != LM_MAX; ++i )
		{
			m_countMessage[i] = 0;
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	LoggerEngine::~LoggerEngine()
	{
		for( TVectorLoggers::iterator 
			it = m_loggers.begin(), 
			it_end = m_loggers.end();
		it != it_end;
		++it )
		{
            const LoggerInterfacePtr & logger = *it;

			logger->flush();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerEngine::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
    //////////////////////////////////////////////////////////////////////////
    void LoggerEngine::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
	//////////////////////////////////////////////////////////////////////////
	bool LoggerEngine::validMessage( EMessageLevel _level, uint32_t _flag ) const
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
	void LoggerEngine::logMessage( EMessageLevel _level, uint32_t _flag, const char * _message, size_t _size  )
	{
		++m_countMessage[_level];

		for( TVectorLoggers::iterator 
			it = m_loggers.begin(), 
			it_end = m_loggers.end();
		it != it_end;
		++it )
		{
			const LoggerInterfacePtr & logger = *it;

			if( logger->validMessage( _level, _flag ) == false )
			{
				continue;
			}

			logger->log( _level, _flag, _message, _size );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t LoggerEngine::getCountMessage( Mengine::EMessageLevel _level )
	{
		size_t count = m_countMessage[_level];

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoggerEngine::registerLogger( const LoggerInterfacePtr & _logger )
	{
		TVectorLoggers::iterator it_find = 
			std::find( m_loggers.begin(), m_loggers.end(), _logger );

		if( it_find != m_loggers.end() )
		{
			return false;
		}
        
		if( _logger->initialize() == false )
		{
			return false;
		}

		m_loggers.emplace_back( _logger );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoggerEngine::unregisterLogger( const LoggerInterfacePtr & _logger )
	{
		TVectorLoggers::iterator it_find = 
			std::find( m_loggers.begin(), m_loggers.end(), _logger );

		if( it_find == m_loggers.end() )
		{
			return false;
		}

		_logger->flush();

		_logger->finalize();

		m_loggers.erase( it_find );

        return true;
	}
}
