#include "LoggerBase.h"

#include "Interface/LoggerServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerBase::LoggerBase()
        : m_writeHistory( false )
        , m_color( LCOLOR_NONE )
        , m_verboseLevel( LM_VERBOSE )
        , m_verboseFilter( 0xFFFFFFFF )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerBase::~LoggerBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::initializeLogger()
    {
        if( this->_initializeLogger() == false )
        {
            return false;
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
    bool LoggerBase::validMessage( ELoggerLevel _level, uint32_t _filter ) const
    {
        if( m_verboseLevel < _level )
        {
            return false;
        }

        if( _filter == 0 )
        {
            return true;
        }

        if( (m_verboseFilter & _filter) == 0 )
        {
            return false;
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
