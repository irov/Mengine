#	include "HTTPLogger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HTTPLogger::HTTPLogger( HTTPSystemInterface * _interfaceHTTP )
		: m_interface(_interfaceHTTP)
        , m_verboseLevel(LM_INFO)
        , m_verboseFlag(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	HTTPLogger::~HTTPLogger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void HTTPLogger::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level; 
	}
    //////////////////////////////////////////////////////////////////////////
    void HTTPLogger::setVerboseFlag( size_t _flag )
    {
        m_verboseFlag = _flag;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HTTPLogger::validMessage( EMessageLevel _level, size_t _flag ) const
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
}

