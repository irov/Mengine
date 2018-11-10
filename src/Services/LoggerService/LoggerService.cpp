#include "LoggerService.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LoggerService, Mengine::LoggerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerService::LoggerService()
        : m_verboseLevel( LM_ERROR )
        , m_verboseFlag( 0 )
    {
        for( uint32_t i = 0; i != LM_MAX; ++i )
        {
            m_countMessage[i] = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerService::~LoggerService()
    {
        for( const LoggerInterfacePtr & logger : m_loggers )
        {
            logger->flush();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::setVerboseLevel( EMessageLevel _level )
    {
        m_verboseLevel = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::validMessage( EMessageLevel _level, uint32_t _flag ) const
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
    void LoggerService::logMessage( EMessageLevel _level, uint32_t _flag, const char * _message, uint32_t _size )
    {
        ++m_countMessage[_level];

        for( const LoggerInterfacePtr & logger : m_loggers )
        {
            if( logger->validMessage( _level, _flag ) == false )
            {
                continue;
            }

            logger->log( _level, _flag, _message, _size );
        }

#ifdef MENGINE_LOGGER_HISTORY
        Record history;
        history.level = _level;
        history.flag = _flag;
        history.message.assign( _message, _size );

        m_history.emplace_back( history );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t LoggerService::getCountMessage( EMessageLevel _level )
    {
        uint32_t count = m_countMessage[_level];

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::writeHistory( const LoggerInterfacePtr & _logger ) const
    {
        (void)_logger;

#ifdef MENGINE_LOGGER_HISTORY
        for( const Record & record : m_history )
        {
            const Char * record_message_str = record.message.c_str();
            uint32_t record_message_size = record.message.size();

            _logger->log( record.level, record.flag, record_message_str, record_message_size );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::registerLogger( const LoggerInterfacePtr & _logger )
    {
        VectorLoggers::iterator it_find =
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
    bool LoggerService::unregisterLogger( const LoggerInterfacePtr & _logger )
    {
        VectorLoggers::iterator it_find =
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
