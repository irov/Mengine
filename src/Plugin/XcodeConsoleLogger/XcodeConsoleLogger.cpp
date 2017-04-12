#	include "XcodeConsoleLogger.h"

#	include <cstdio>
#	include <locale.h>

#   include <iostream>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	XcodeConsoleLogger::XcodeConsoleLogger()
        : m_verboseLevel( LM_INFO )
        , m_verboseFlag( 0xFFFFFFFF )
    {
    }
    //////////////////////////////////////////////////////////////////////////
	XcodeConsoleLogger::~XcodeConsoleLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool XcodeConsoleLogger::initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XcodeConsoleLogger::finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void XcodeConsoleLogger::setVerboseLevel( EMessageLevel _level )
    {
        m_verboseLevel = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    void XcodeConsoleLogger::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XcodeConsoleLogger::validMessage( EMessageLevel _level, uint32_t _flag ) const
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
    void XcodeConsoleLogger::log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count )
    {
		(void)_level;
        (void)_flag;

		std::cout.write( _data, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void XcodeConsoleLogger::flush()
    {
        std::cout.flush();
    }
}	// namespace Menge
