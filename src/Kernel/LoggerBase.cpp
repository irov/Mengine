#include "LoggerBase.h"

#include "Interface/LoggerServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerBase::LoggerBase()
        : m_color( LCOLOR_NONE )
        , m_verboseLevel( LM_UNKNOWN )
        , m_verboseFilter( 0xFFFFFFFF )
        , m_writeHistory( false )
        , m_supportVerboses( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerBase::~LoggerBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::initializeLogger( ELoggerLevel _verboseLevel )
    {
        if( this->_initializeLogger() == false )
        {
            return false;
        }

        if( m_verboseLevel == LM_UNKNOWN )
        {
            m_verboseLevel = _verboseLevel;
        }

        if( m_writeHistory == true )
        {
            LOGGER_SERVICE()
                ->writeHistory( LoggerInterfacePtr::from( this ) );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::finalizeLogger()
    {
        this->_finalizeLogger();
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::_initializeLogger()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::_finalizeLogger()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::setWriteHistory( bool _writeHistory )
    {
        m_writeHistory = _writeHistory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::getWriteHistory() const
    {
        return m_writeHistory;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::setSupportVerboses( bool _supportVerboses )
    {
        m_supportVerboses = _supportVerboses;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::setSupportVerboses() const
    {
        return m_supportVerboses;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::setColor( uint32_t _color )
    {
        m_color = _color;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t LoggerBase::getColor() const
    {
        return m_color;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::setVerboseLevel( ELoggerLevel _level )
    {
        m_verboseLevel = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    ELoggerLevel LoggerBase::getVerboseLevel() const
    {
        return m_verboseLevel;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::setVerboseFilter( uint32_t _filter )
    {
        m_verboseFilter = _filter;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t LoggerBase::getVerboseFilter() const
    {
        return m_verboseFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::validMessage( const LoggerMessage & _message ) const
    {
        if( m_verboseLevel < _message.level )
        {
            if( m_supportVerboses == false )
            {
                return false;
            }

            if( LOGGER_SERVICE()
                ->validateVerbose( _message.category ) == false )
            {
                return false;
            }
        }

        if( _message.filter != 0 )
        {
            if( (m_verboseFilter & _message.filter) == 0 )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::flush()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
