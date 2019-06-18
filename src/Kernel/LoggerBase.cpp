#include "LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerBase::LoggerBase()
        : m_verboseLevel( LM_INFO )
        , m_verboseFlag( 0xFFFFFFFF )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerBase::~LoggerBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::setVerboseLevel( EMessageLevel _level )
    {
        m_verboseLevel = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::validMessage( EMessageLevel _level, uint32_t _flag ) const
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
    void LoggerBase::flush()
    {
    }
}
