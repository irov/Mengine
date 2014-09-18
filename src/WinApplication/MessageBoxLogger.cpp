#	include "MessageBoxLogger.h"

#   include "Interface/UnicodeInterface.h"

#	include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MessageBoxLogger::MessageBoxLogger( ServiceProviderInterface * _serviceProvider )
		: m_serviceProvider(_serviceProvider)
		, m_verboseLevel(LM_INFO)
		, m_verboseFlag(0xFFFFFFFF)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MessageBoxLogger::~MessageBoxLogger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MessageBoxLogger::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
    //////////////////////////////////////////////////////////////////////////
    void MessageBoxLogger::setVerboseFlag( size_t _flag )
    {
        m_verboseFlag = _flag;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MessageBoxLogger::validMessage( EMessageLevel _level, size_t _flag ) const
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
	void MessageBoxLogger::log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count )
	{	
		(void)_level;
        (void)_flag;
		(void)_count;
		

		MessageBoxA( NULL, _data, "Critical Error", MB_OK );
	}
	//////////////////////////////////////////////////////////////////////////
	void MessageBoxLogger::flush()
	{
	}
}	// namespace Menge
